#include "ShadowMapManager.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "base/Logger.h"
#include "base/DirectXCommon.h"
#include "manager/CameraManager.h"
#include  "manager/SrvManager.h"
#include "scene/manager/SceneManager.h"
#include "lighting/LightManager.h"

ShadowMapManager* ShadowMapManager::instance_ = nullptr;

ShadowMapManager* ShadowMapManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ShadowMapManager();
	}
	return instance_;
}

void ShadowMapManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, SceneManager* sceneManager,
								  LightManager* lightManager)
{
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;
	sceneManager_ = sceneManager;
	lightManager_ = lightManager;
	// シャドウマップパイプラインの初期化
	shadowMapPipeline_ = std::make_unique<ShadowMapPipeline>();
	shadowMapPipeline_->Initialize(dxCommon);

	// サンプラーヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC samplerDesc = {};
	samplerDesc.NumDescriptors = 1;
	samplerDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	samplerDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	dxCommon_->GetDevice()->CreateDescriptorHeap(&samplerDesc, IID_PPV_ARGS(&shadowSamplerHeap_));

	// 比較サンプラー作成
	D3D12_SAMPLER_DESC compSamplerDesc = {};
	compSamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	compSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	compSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	compSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	compSamplerDesc.BorderColor[0] = 1.0f;
	compSamplerDesc.BorderColor[1] = 1.0f;
	compSamplerDesc.BorderColor[2] = 1.0f;
	compSamplerDesc.BorderColor[3] = 1.0f;
	compSamplerDesc.MinLOD = 0;
	compSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	compSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	shadowSamplerHandle_ = shadowSamplerHeap_->GetCPUDescriptorHandleForHeapStart();
	shadowSamplerGpuHandle_ = shadowSamplerHeap_->GetGPUDescriptorHandleForHeapStart();

	dxCommon_->GetDevice()->CreateSampler(&compSamplerDesc, shadowSamplerHandle_);

	Logger::Log("ShadowMapManager initialized");
}

// 
ShadowMapData ShadowMapManager::CreateShadowMapResource()
{
	ShadowMapData data;

	// シャドウマップ用のテクスチャを作成
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = ShadowMapData::kShadowMapSize;
	depthResourceDesc.Height = ShadowMapData::kShadowMapSize;
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.MipLevels = 1;
	depthResourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.SampleDesc.Quality = 0;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 深度バッファのクリア値
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;

	// ヒーププロパティ設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// リソース作成
	HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&data.shadowMapTexture)
	);

	if (FAILED(hr))
	{
		Logger::Log("Failed to create shadow map texture");
		return data;
	}

	// DSVヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hr = dxCommon_->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&data.dsvHeap));

	if (FAILED(hr))
	{
		Logger::Log("Failed to create shadow map DSV heap");
		return data;
	}

	// DSVの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	data.dsvHandle = data.dsvHeap->GetCPUDescriptorHandleForHeapStart();
	dxCommon_->GetDevice()->CreateDepthStencilView(
		data.shadowMapTexture.Get(),
		&dsvDesc,
		data.dsvHandle
	);

	// SRVの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;

	// SRVマネージャーを使ってSRVを作成
	uint32_t newIndex = srvManager_->Allocate();
	srvManager_->CreateSRVforTexture2D(
		newIndex,
		data.shadowMapTexture.Get(),
		srvDesc.Format,
		1 // mipLevels
	);
	data.srvIndex = newIndex;

	// ビューポートとシザー矩形の設定
	data.viewport.TopLeftX = 0.0f;
	data.viewport.TopLeftY = 0.0f;
	data.viewport.Width = static_cast<float>(ShadowMapData::kShadowMapSize);
	data.viewport.Height = static_cast<float>(ShadowMapData::kShadowMapSize);
	data.viewport.MinDepth = 0.0f;
	data.viewport.MaxDepth = 1.0f;

	data.scissorRect.left = 0;
	data.scissorRect.top = 0;
	data.scissorRect.right = ShadowMapData::kShadowMapSize;
	data.scissorRect.bottom = ShadowMapData::kShadowMapSize;

	return data;
}

void ShadowMapManager::Finalize()
{
	delete instance_;
	instance_ = nullptr;

	Logger::Log("ShadowMapManager finalized");
}

// スポットライト用シャドウマップの作成
void ShadowMapManager::CreateSpotLightShadowMap(const std::string& name)
{
	// 既に存在する場合は何もしない
	auto it = spotLightShadowMaps_.find(name);
	if (it != spotLightShadowMaps_.end())
	{
		return;
	}

	// 新しいシャドウマップを作成
	auto shadowMapData = std::make_unique<ShadowMapData>(CreateShadowMapResource());
	shadowMapData->type = ShadowMapType::SpotLight;

	// マップに追加
	spotLightShadowMaps_[name] = std::move(shadowMapData);

	Logger::Log("Created spot light shadow map: " + name);
}

// ポイントライト用シャドウマップの作成
void ShadowMapManager::CreatePointLightShadowMap(const std::string& name)
{
	// 既に存在する場合は何もしない
	auto it = pointLightShadowMaps_.find(name);
	if (it != pointLightShadowMaps_.end())
	{
		return;
	}

	// キューブマップ用のデータを作成
	auto cubeMapData = std::make_unique<CubeShadowMapData>();

	// 6面分のシャドウマップを作成
	for (int i = 0; i < 6; i++)
	{
		cubeMapData->faces[i] = CreateShadowMapResource();
		cubeMapData->faces[i].type = ShadowMapType::PointLight;
	}

	// マップに追加
	pointLightShadowMaps_[name] = std::move(cubeMapData);

	Logger::Log("Created point light shadow map: " + name);
}

// ディレクショナルライト用シャドウマップの作成
void ShadowMapManager::CreateDirectionalLightShadowMap(
	const std::string& name,
	const std::array<float, ShadowMapData::kCascadeCount>& cascadeDistances)
{
	// 既に存在する場合は何もしない
	auto it = directionalLightShadowMaps_.find(name);
	if (it != directionalLightShadowMaps_.end())
	{
		return;
	}

	// カスケードシャドウマップ用のデータを作成
	auto cascadeMapData = std::make_unique<CascadeShadowMapData>();
	cascadeMapData->cascadeDistances = cascadeDistances;

	// カスケードごとのシャドウマップを作成
	for (int i = 0; i < ShadowMapData::kCascadeCount; i++)
	{
		cascadeMapData->cascades[i] = CreateShadowMapResource();
		cascadeMapData->cascades[i].type = ShadowMapType::DirectionalLight;
	}

	// マップに追加
	directionalLightShadowMaps_[name] = std::move(cascadeMapData);

	Logger::Log("Created directional light shadow map: " + name);
}

// スポットライト用シャドウマップパスの開始
void ShadowMapManager::BeginSpotLightShadowPass(const std::string& name)
{
	auto it = spotLightShadowMaps_.find(name);
	if (it == spotLightShadowMaps_.end())
	{
		Logger::Log("Shadow map not found: " + name);
		return;
	}

	const auto& shadowMap = it->second;
	ID3D12GraphicsCommandList* cmdList = dxCommon_->GetCommandList();

	// レンダーターゲットをシャドウマップに設定（深度のみ）
	cmdList->OMSetRenderTargets(0, nullptr, FALSE, &shadowMap->dsvHandle);

	// 深度バッファのクリア
	cmdList->ClearDepthStencilView(shadowMap->dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ビューポートとシザー矩形の設定
	cmdList->RSSetViewports(1, &shadowMap->viewport);
	cmdList->RSSetScissorRects(1, &shadowMap->scissorRect);

	// シャドウマップパイプラインの設定
	cmdList->SetPipelineState(shadowMapPipeline_->GetPipelineState());
}

// ポイントライト用シャドウマップパスの開始
void ShadowMapManager::BeginPointLightShadowPass(const std::string& name, uint32_t faceIndex)
{
	if (faceIndex >= 6)
	{
		Logger::Log("Invalid face index for point light shadow map");
		return;
	}

	auto it = pointLightShadowMaps_.find(name);
	if (it == pointLightShadowMaps_.end())
	{
		Logger::Log("Shadow map not found: " + name);
		return;
	}

	const auto& shadowMap = it->second->faces[faceIndex];
	ID3D12GraphicsCommandList* cmdList = dxCommon_->GetCommandList();

	// レンダーターゲットをシャドウマップに設定（深度のみ）
	cmdList->OMSetRenderTargets(0, nullptr, FALSE, &shadowMap.dsvHandle);

	// 深度バッファのクリア
	cmdList->ClearDepthStencilView(shadowMap.dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ビューポートとシザー矩形の設定
	cmdList->RSSetViewports(1, &shadowMap.viewport);
	cmdList->RSSetScissorRects(1, &shadowMap.scissorRect);

	// シャドウマップパイプラインの設定
	cmdList->SetPipelineState(shadowMapPipeline_->GetPipelineState());
	cmdList->SetGraphicsRootSignature(shadowMapPipeline_->GetRootSignature());
}

// ディレクショナルライト用シャドウマップパスの開始
void ShadowMapManager::BeginDirectionalLightShadowPass(const std::string& name, uint32_t cascadeIndex)
{
	if (cascadeIndex >= ShadowMapData::kCascadeCount)
	{
		Logger::Log("Invalid cascade index for directional light shadow map");
		return;
	}

	auto it = directionalLightShadowMaps_.find(name);
	if (it == directionalLightShadowMaps_.end())
	{
		Logger::Log("Shadow map not found: " + name);
		return;
	}

	const auto& shadowMap = it->second->cascades[cascadeIndex];
	ID3D12GraphicsCommandList* cmdList = dxCommon_->GetCommandList();

	// レンダーターゲットをシャドウマップに設定（深度のみ）
	cmdList->OMSetRenderTargets(0, nullptr, FALSE, &shadowMap.dsvHandle);

	// 深度バッファのクリア
	cmdList->ClearDepthStencilView(shadowMap.dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ビューポートとシザー矩形の設定
	cmdList->RSSetViewports(1, &shadowMap.viewport);
	cmdList->RSSetScissorRects(1, &shadowMap.scissorRect);

	// シャドウマップパイプラインの設定
	cmdList->SetPipelineState(shadowMapPipeline_->GetPipelineState());
	cmdList->SetGraphicsRootSignature(shadowMapPipeline_->GetRootSignature());
}

// シャドウマップパスの終了
void ShadowMapManager::EndShadowPass()
{
	// リソースバリアの追加など、必要に応じて実装
	// 現在の単純な実装では特に何もする必要がない
}

// すべてのシャドウマップのレンダリング
void ShadowMapManager::RenderAllShadowMaps()
{
	// シーンマネージャーとライトマネージャーの設定確認
	if (!sceneManager_ || !lightManager_)
	{
		if (!sceneManager_) Logger::Log("SceneManager not set in ShadowMapManager");
		if (!lightManager_) Logger::Log("LightManager not set in ShadowMapManager");
		return;
	}

	// 描画するシャドウマップがない場合は早期リターン
	if (spotLightShadowMaps_.empty() &&
		pointLightShadowMaps_.empty() &&
		directionalLightShadowMaps_.empty())
	{
		return;
	}

	// パイプラインを一度だけセット
	ID3D12GraphicsCommandList* cmdList = dxCommon_->GetCommandList();
	cmdList->SetPipelineState(shadowMapPipeline_->GetPipelineState());
	cmdList->SetGraphicsRootSignature(shadowMapPipeline_->GetRootSignature());

	// スポットライト用シャドウマップの描画
	for (const auto& pair : spotLightShadowMaps_)
	{
		const std::string& name = pair.first;
		const auto& shadowMap = pair.second;

		// レンダーターゲット、ビューポート、シザー矩形の設定とクリア
		cmdList->OMSetRenderTargets(0, nullptr, FALSE, &shadowMap->dsvHandle);
		cmdList->ClearDepthStencilView(shadowMap->dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		cmdList->RSSetViewports(1, &shadowMap->viewport);
		cmdList->RSSetScissorRects(1, &shadowMap->scissorRect);

		// ライトからのビュープロジェクション行列を計算
		Matrix4x4 lightViewProj = CalculateSpotLightViewProjection(name);

		// シーンをシャドウマップに描画
		sceneManager_->GetCurrentScene()->DrawForShadow(ToXMMATRIX(lightViewProj));
	}

	// ポイントライト用シャドウマップの描画（6面）
	for (const auto& pair : pointLightShadowMaps_)
	{
		const std::string& name = pair.first;

		// 6面それぞれに対して描画
		for (uint32_t face = 0; face < 6; face++)
		{
			const auto& shadowMap = pair.second->faces[face];

			// レンダーターゲット、ビューポート、シザー矩形の設定とクリア
			cmdList->OMSetRenderTargets(0, nullptr, FALSE, &shadowMap.dsvHandle);
			cmdList->ClearDepthStencilView(shadowMap.dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			cmdList->RSSetViewports(1, &shadowMap.viewport);
			cmdList->RSSetScissorRects(1, &shadowMap.scissorRect);

			// ライトからのビュープロジェクション行列を計算
			Matrix4x4 lightViewProj = CalculatePointLightViewProjection(name, face);

			// シーンをシャドウマップに描画
			sceneManager_->GetCurrentScene()->DrawForShadow(ToXMMATRIX(lightViewProj));
		}
	}

	// ディレクショナルライト用シャドウマップの描画（カスケード）
	for (const auto& pair : directionalLightShadowMaps_)
	{
		const std::string& name = pair.first;

		// 各カスケードに対して描画
		for (uint32_t cascade = 0; cascade < ShadowMapData::kCascadeCount; cascade++)
		{
			const auto& shadowMap = pair.second->cascades[cascade];

			// レンダーターゲット、ビューポート、シザー矩形の設定とクリア
			cmdList->OMSetRenderTargets(0, nullptr, FALSE, &shadowMap.dsvHandle);
			cmdList->ClearDepthStencilView(shadowMap.dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			cmdList->RSSetViewports(1, &shadowMap.viewport);
			cmdList->RSSetScissorRects(1, &shadowMap.scissorRect);

			// ライトからのビュープロジェクション行列を計算
			Matrix4x4 lightViewProj = CalculateDirectionalLightViewProjection(name, cascade);

			// シーンをシャドウマップに描画
			sceneManager_->GetCurrentScene()->DrawForShadow(ToXMMATRIX(lightViewProj));
		}
	}
}

// スポットライトのビュープロジェクション行列計算

Matrix4x4 ShadowMapManager::CalculateSpotLightViewProjection(const std::string& name)
{
	// LightManagerがセットされていない場合は単位行列を返す
	if (!lightManager_)
	{
		Logger::Log("LightManager not set in ShadowMapManager");
		return MakeIdentity4x4();
	}

	// LightManagerからスポットライト情報を取得
	const GPUSpotLight& spotLight = lightManager_->GetSpotLight(name);

	// ライトの位置と方向からビュー行列を計算
	Vector3 lightPos = spotLight.position;
	Vector3 lightDir = spotLight.direction;
	Vector3 up = Vector3(0, 1, 0); // 上方向

	// ライトが真上または真下を向いている場合は別の上方向を使用
	if (std::abs(lightDir.y) > 0.99f)
	{
		up = Vector3(0, 0, 1);
	}

	// ライト視点からのビュー行列を計算
	Matrix4x4 viewMatrix = MakeLookAtMatrix(lightPos, lightPos + lightDir, up);

	// プロジェクション行列
	float lightAngle = std::acos(spotLight.cosAngle);
	Matrix4x4 projMatrix = MakePerspectiveFovMatrix(lightAngle * 2.0f, 1.0f, 0.1f, spotLight.distance);

	// ビュープロジェクション行列を返す
	return viewMatrix * projMatrix;
}


// ポイントライトのビュープロジェクション行列計算
Matrix4x4 ShadowMapManager::CalculatePointLightViewProjection(const std::string& name, uint32_t faceIndex)
{
	if (!lightManager_)
	{
		Logger::Log("LightManager not set in ShadowMapManager");
		return MakeIdentity4x4();
	}

	// ポイントライトは6方向それぞれの視点が必要
	const GPUPointLight& pointLight = lightManager_->GetPointLight(name);

	Vector3 lightPos = pointLight.position;

	// キューブマップの6面それぞれの方向と上ベクトル
	Vector3 directions[6] = {
		{ 1.0f, 0.0f, 0.0f },  // +X
		{ -1.0f, 0.0f, 0.0f }, // -X
		{ 0.0f, 1.0f, 0.0f },  // +Y
		{ 0.0f, -1.0f, 0.0f }, // -Y
		{ 0.0f, 0.0f, 1.0f },  // +Z
		{ 0.0f, 0.0f, -1.0f }  // -Z
	};

	Vector3 ups[6] = {
		{ 0.0f, 1.0f, 0.0f },  // +X
		{ 0.0f, 1.0f, 0.0f },  // -X
		{ 0.0f, 0.0f, -1.0f }, // +Y
		{ 0.0f, 0.0f, 1.0f },  // -Y
		{ 0.0f, 1.0f, 0.0f },  // +Z
		{ 0.0f, 1.0f, 0.0f }   // -Z
	};

	// ビュー行列を計算
	Matrix4x4 viewMatrix = MakeLookAtMatrix(
		lightPos,
		lightPos + directions[faceIndex],
		ups[faceIndex]
	);

	// 正方形のビューにするため、90度のFOVでプロジェクション行列を作成
	Matrix4x4 projMatrix = MakePerspectiveFovMatrix(
		3.14159f / 2.0f, // 90度
		1.0f, // アスペクト比1:1
		0.1f, // 近クリップ面
		pointLight.radius // 遠クリップ面
	);

	return viewMatrix * projMatrix;
}

// ディレクショナルライトのビュープロジェクション行列計算
Matrix4x4 ShadowMapManager::CalculateDirectionalLightViewProjection(const std::string& name, uint32_t cascadeIndex)
{
	//// カスケードシャドウマップ用の計算
	//const DirectionalLight& dirLight = lightManager_->

	//auto it = directionalLightShadowMaps_.find(name);
	//if (it == directionalLightShadowMaps_.end())
	//{
	//	// 見つからない場合は単位行列を返す
	//	return MakeIdentity4x4();
	//}

	//// カスケードごとの距離を取得
	//float cascadeDistance = it->second->cascadeDistances[cascadeIndex];

	//// カメラの情報を取得（実際のアプリケーションに合わせて調整）
	//Vector3 cameraPos = sceneManager_->GetCameraManager()->GetActiveCamera()->GetTranslate();
	//Vector3 cameraDir = sceneManager_->GetCameraManager()->GetActiveCamera()->GetRotate();

	//// 現在のカスケードの視錐台中心位置（カメラの前方）
	//Vector3 cascadeCenterPos = cameraPos + cameraDir * (cascadeDistance * 0.5f);

	//// ライトの方向
	//Vector3 lightDir = dirLight.direction;

	//// ライトの位置はカスケード中心から逆方向に
	//Vector3 lightPos = cascadeCenterPos - lightDir * cascadeDistance;

	//// ライト視点のビュー行列
	//Vector3 up = Vector3(0, 1, 0); // 上方向
	//if (std::abs(lightDir.y) > 0.99f)
	//{
	//	up = Vector3(0, 0, 1); // ライトが真上/真下を向いている場合
	//}
	//Matrix4x4 viewMatrix = MakeLookAtMatrix(lightPos, lightPos + lightDir, up);

	//// カスケードごとに視錐台サイズを変えるオーソグラフィックプロジェクション
	//float shadowExtent = cascadeDistance;
	//Matrix4x4 projMatrix = MakeOrthographicMatrix(
	//	-shadowExtent, shadowExtent,
	//	-shadowExtent, shadowExtent,
	//	0.1f, cascadeDistance * 2.0f
	//);

	//return viewMatrix * projMatrix;
}

// シャドウマップSRVのインデックスを取得
uint32_t ShadowMapManager::GetSpotLightShadowMapSrvIndex(const std::string& name) const
{
	auto it = spotLightShadowMaps_.find(name);
	if (it == spotLightShadowMaps_.end())
	{
		Logger::Log("Shadow map not found: " + name);
		return 0; // 無効なインデックス
	}
	return it->second->srvIndex;
}

uint32_t ShadowMapManager::GetPointLightShadowMapSrvIndex(const std::string& name, uint32_t faceIndex) const
{
	if (faceIndex >= 6)
	{
		Logger::Log("Invalid face index for point light shadow map");
		return 0;
	}

	auto it = pointLightShadowMaps_.find(name);
	if (it == pointLightShadowMaps_.end())
	{
		Logger::Log("Shadow map not found: " + name);
		return 0; // 無効なインデックス
	}
	return it->second->faces[faceIndex].srvIndex;
}

uint32_t ShadowMapManager::GetDirectionalLightShadowMapSrvIndex(const std::string& name, uint32_t cascadeIndex) const
{
	if (cascadeIndex >= ShadowMapData::kCascadeCount)
	{
		Logger::Log("Invalid cascade index for directional light shadow map");
		return 0;
	}

	auto it = directionalLightShadowMaps_.find(name);
	if (it == directionalLightShadowMaps_.end())
	{
		Logger::Log("Shadow map not found: " + name);
		return 0; // 無効なインデックス
	}
	return it->second->cascades[cascadeIndex].srvIndex;
}

// シャドウマップの削除
void ShadowMapManager::RemoveSpotLightShadowMap(const std::string& name)
{
	auto it = spotLightShadowMaps_.find(name);
	if (it != spotLightShadowMaps_.end())
	{
		spotLightShadowMaps_.erase(it);
		Logger::Log("Removed spot light shadow map: " + name);
	}
}

void ShadowMapManager::RemovePointLightShadowMap(const std::string& name)
{
	auto it = pointLightShadowMaps_.find(name);
	if (it != pointLightShadowMaps_.end())
	{
		pointLightShadowMaps_.erase(it);
		Logger::Log("Removed point light shadow map: " + name);
	}
}

void ShadowMapManager::RemoveDirectionalLightShadowMap(const std::string& name)
{
	auto it = directionalLightShadowMaps_.find(name);
	if (it != directionalLightShadowMaps_.end())
	{
		directionalLightShadowMaps_.erase(it);
		Logger::Log("Removed directional light shadow map: " + name);
	}
}