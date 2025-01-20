#include "ParticleManager.h"

#include <dxcapi.h>

#include "TextureManager.h"
#include "3d/ModelManager.h"
#include "base/DirectXCommon.h"
#include "base/Logger.h"
#include "manager/SrvManager.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

ParticleManager* ParticleManager::instance_ = nullptr;

ParticleManager* ParticleManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ParticleManager();
	}
	return instance_;
}

void ParticleManager::Finalize()
{
	if (instance_ != nullptr)
	{
		//パーティクルグループの解放
		delete instance_;
		instance_ = nullptr;

	}
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	//引数をメンバ変数に記録
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	//ランダムエンジンの初期化
	std::random_device rd;
	mt_.seed(rd());

	//パーティクルグループの初期化
	particleGroups_.clear();

	//マテリアルデータの生成
	CreateMaterialData();

	//モデルデータの初期化
	InitializeModelData();

	//頂点データの生成
	CreateVertexData();

	//パイプラインの生成
	CreateGraphicsPipelineState();


}

void ParticleManager::Update(CameraManager* camera)
{
#ifdef _DEBUG
	/*--------------[ ImGui ]-----------------*/
	ImGui::Begin("ParticleManager");
	for (auto& groupPair : particleGroups_)
	{
		for (auto& particle : groupPair.second.particles)
		{
			ImGui::Text("Particle");
			ImGui::Text("Position: %f %f %f", particle.transform.translate.x, particle.transform.translate.y, particle.transform.translate.z);
			ImGui::Text("Velocity: %f %f %f", particle.velocity.x, particle.velocity.y, particle.velocity.z);
			ImGui::Text("LifeTime: %f", particle.lifeTime);
			ImGui::Text("CurrentTime: %f", particle.currentTime);
			ImGui::Text("Color: %f %f %f %f", particle.color.x, particle.color.y, particle.color.z, particle.color.w);
			ImGui::Separator();
		}
	}
	ImGui::End();
#endif


	const float kDeltaTime = 1.0f / 60.0f;
	// ビルボード用の行列計算
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(0.0f);
	Matrix4x4 billboardMatrix = MakeIdentity4x4();

	// カメラの回転を適用
	billboardMatrix = Multiply(backToFrontMatrix, camera->GetActiveCamera()->GetWorldMatrix());

	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	// particleGroups での処理
	for (auto& groupPair : particleGroups_) {
		ParticleGroup& group = groupPair.second;
		group.instanceCount = 0; // インスタンスカウントをリセット


		// 各グループ内のパーティクルを処理
		for (std::list<Particle>::iterator particleItr = group.particles.begin(); particleItr != group.particles.end();)
		{
			if (particleItr->lifeTime <= particleItr->currentTime)
			{
				particleItr = group.particles.erase(particleItr);
				continue;
			}

			// パーティクルの位置更新
			particleItr->transform.translate += particleItr->velocity * kDeltaTime;
			particleItr->currentTime += kDeltaTime;

			// パーティクルの透明度計算
			float alpha = 1.0f - (particleItr->currentTime / particleItr->lifeTime);
			materialData_->color = { 1.0f,1.0f,1.0f,alpha };

			// インスタンス数の制限を守る
			if (group.instanceCount < kMaxParticleCount)
			{
				// スケール行列と移動行列の生成
				Matrix4x4 scaleMatrix = MakeScaleMatrix(particleItr->transform.scale);
				Matrix4x4 translateMatrix = MakeTranslateMatrix(particleItr->transform.translate);

				// 世界行列とWVP行列の計算
				Matrix4x4 worldMatrixInstancing = scaleMatrix * billboardMatrix * translateMatrix;
				Matrix4x4 worldViewProjectionMatrixInstancing = Multiply(worldMatrixInstancing, Multiply(camera->GetActiveCamera()->GetViewMatrix(), camera->GetActiveCamera()->GetProjectionMatrix()));

				// インスタンシングデータの設定
				group.instancingData[group.instanceCount].WVP = worldViewProjectionMatrixInstancing;
				group.instancingData[group.instanceCount].World = worldMatrixInstancing;
				group.instancingData[group.instanceCount].color = particleItr->color;
				group.instancingData[group.instanceCount].color.w = alpha;
				// インスタンスの数をインクリメント
				++group.instanceCount;
			}
			// 次のパーティクルに進める
			++particleItr;
		}
	}
}

void ParticleManager::Draw()
{
	/*--------------[ ルートシグネチャの設定 ]-----------------*/

	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	/*--------------[ パイプラインステートの設定 ]-----------------*/

	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());

	/*--------------[ プリミティブトポロジーの設定 ]-----------------*/

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*--------------[ VBVを設定 ]-----------------*/

	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	for (auto& groupPair : particleGroups_)
	{
		ParticleGroup& group = groupPair.second;
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		// インスタンシングデータのSRVのDescriptorTableの先頭を設定
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(group.instancingSrvIndex));
		// SRVのDescriptorTableの先頭を設定
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(group.materialData.textureIndex));

		// 描画
		dxCommon_->GetCommandList()->DrawInstanced(6, group.instanceCount, 0, 0);
	}

}

void ParticleManager::CreateVertexData()
{
	/*--------------[ VertexResourceを作る ]-----------------*/

	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());//１頂点当たりのサイズ

	/*--------------[ VertexBufferViewを設定する ]-----------------*/

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();				//リソースの先頭のアドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);									//１頂点当たりのサイズ

	/*--------------[ VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる ]-----------------*/

	vertexResource_->Map(0,
		nullptr,
		reinterpret_cast<void**>(&vertexData)
	);

	//頂点データをリソースにコピー
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void ParticleManager::CreateMaterialData()
{
	//マテリアル用のリソースを作成
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));

	//書き込むためのアドレスを取得
	materialResource_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(&materialData_)
	);
	//マテリアルデータの初期化
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();

}

void ParticleManager::InitializeModelData()
{
	modelData_.vertices.push_back({ .position = {1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f, -1.0f, 0.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} });
}


void ParticleManager::CreateParticleGroup(const std::string& groupName, const std::string& textureFilePath)
{
	//パーティクルグループが既に存在する場合はエラー
	if (particleGroups_.find(groupName) != particleGroups_.end())
	{
		// ログ出力
		Logger::Log("Particle group with name " + groupName + " already exists.");
		assert(false);
	}

	//パーティクルグループを作成し、コンテナに追加
	ParticleGroup newGroup;
	newGroup.materialData.textureFilePath = textureFilePath;
	//テクスチャを読み込む
	TextureManager::GetInstance()->LoadTexture(newGroup.materialData.textureFilePath);
	//マテリアルデータにテクスチャのSRVインデックスを設定
	newGroup.materialData.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(newGroup.materialData.textureFilePath);
	//インスタンシング用のリソースを作成
	newGroup.instancingResource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kMaxParticleCount);
	newGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&newGroup.instancingData));

	//インスタンシング用にSRVを確保してSRVインデックスを記録
	newGroup.instancingSrvIndex = srvManager_->Allocate() + 1;
	//SRV生成（StructuredBuffer用の設定）
	srvManager_->CreateSRVforStructuredBuffer(
		newGroup.instancingSrvIndex,
		newGroup.instancingResource.Get(),
		kMaxParticleCount, // numElements: パーティクルの最大数
		sizeof(ParticleForGPU) // structureByteStride: 各パーティクルのサイズ
	);

	//for (uint32_t i = 0; i < newGroup.instanceCount; ++i)
	//{
	//	newGroup.particles.push_back(
	//		Particle(
	//			{},
	//			{},
	//			{1.0f,1.0f,1.0f,1.0f},
	//			5.0f,
	//			0.0f
	//	));
	//}

	// 新しいパーティクルグループを追加
	particleGroups_.emplace(groupName, newGroup);
}


void ParticleManager::Emit(const std::string& groupName, const Vector3& position, uint32_t count)
{
	//登録済みのパーティクルグループか確認
	if (particleGroups_.find(groupName) == particleGroups_.end())
	{
		// ログ出力
		Logger::Log("Particle group with name " + groupName + " does not exist.");
		assert(false);
	}

	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	//パーティクルを生成
	for (uint32_t i = 0; i < count; ++i)
	{
		Vector3 randomTranslate = { distribution(mt_),distribution(mt_),distribution(mt_) };
		Vector3 randomVelocity = { distribution(mt_),distribution(mt_),distribution(mt_) };

		particleGroups_[groupName].particles.push_back(
			Particle(
				{ { 1.0f,1.0f,1.0f}, { 0.0f,0.0f,0.0f }, position + randomTranslate },
				Vector3(0.0f, 0.0f, 0.0f) + randomVelocity,
				{ 1.0f,1.0f,1.0f,1.0f },
				5.0f,
				0.0f
			));
	}
}

void ParticleManager::CreateRootSignature()
{
	///===================================================================
	///ディスクリプタレンジの生成
	///===================================================================

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;	//０から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1;		//数は１つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	//SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	///===================================================================
	///RootSignatureを生成する
	///===================================================================

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成。複数設定できるので配列。今回は結果１つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;						//レジスタ番号０とバインド

	/*rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;*/

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;			//descriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;					//vertexShader
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;			//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;					//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);		//Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.RegisterSpace = 0;
	rootParameters[3].Descriptor.ShaderRegister = 1;

	//Smaplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;				//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;			//0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;			//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;							//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;									//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		//PixelShaderを使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	descriptionRootSignature.pParameters = rootParameters;					//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		//配列の長さ

	//シリアライズしてバイナリする
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリをもとに生成
	hr = dxCommon_->GetDevice()->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}



void ParticleManager::CreateGraphicsPipelineState()
{
	//ルートシグネチャ
	CreateRootSignature();

	HRESULT hr;

	///===================================================================
	///InputLayout(インプットレイアウト)
	///===================================================================

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	///===================================================================
	///BlendState(ブレンドステート)
	///===================================================================

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	///===================================================================
	///RasterizerState(ラスタライザステート)
	///===================================================================

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	///===================================================================
	///ShaderをCompileする
	///===================================================================

	//shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileSharder(L"Resources/shaders/Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon_->CompileSharder(L"Resources/shaders/Particle.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	///===================================================================
	///DepthStencilStateの設定を行う
	///===================================================================

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	///===================================================================
	///PSOを生成する
	///===================================================================

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定（気にしなくていい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//実際に生成

	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_)
	);

	assert(SUCCEEDED(hr));
}
