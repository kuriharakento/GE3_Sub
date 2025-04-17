#include "ParticleManager.h"

#include <dxcapi.h>
#include <numbers>

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
			ImGui::Text("Scale: %f %f %f", particle.transform.scale.x, particle.transform.scale.y, particle.transform.scale.z);
			ImGui::Text("Rotation: %f %f %f", particle.transform.rotate.x, particle.transform.rotate.y, particle.transform.rotate.z);
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
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>); // Z軸正方向を基準にする
	Matrix4x4 billboardMatrix = MakeIdentity4x4();

	// カメラの回転を取得
	Matrix4x4 cameraRotationMatrix = camera->GetActiveCamera()->GetWorldMatrix();
	cameraRotationMatrix.m[3][0] = 0.0f;
	cameraRotationMatrix.m[3][1] = 0.0f;
	cameraRotationMatrix.m[3][2] = 0.0f;

	// カメラの回転をビルボード行列に適用
	billboardMatrix = backToFrontMatrix * cameraRotationMatrix;

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
				// 
				Matrix4x4 scaleMatrix = MakeScaleMatrix(particleItr->transform.scale);
				Matrix4x4 translateMatrix = MakeTranslateMatrix(particleItr->transform.translate);
				Matrix4x4 rotateMatrix = MakeRotateMatrix(particleItr->transform.rotate);

				// 世界行列とWVP行列の計算
				Matrix4x4 worldMatrixInstancing = scaleMatrix * rotateMatrix * billboardMatrix * translateMatrix;
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

	for (auto& groupPair : particleGroups_)
	{
		ParticleGroup& group = groupPair.second;
		//頂点バッファビューの設定
		dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &group.vertexBufferView);
		//
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		// インスタンシングデータのSRVのDescriptorTableの先頭を設定
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(group.instancingSrvIndex));
		// SRVのDescriptorTableの先頭を設定
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(group.materialData.textureIndex));
		// 描画
		dxCommon_->GetCommandList()->DrawInstanced(6, group.instanceCount, 0, 0);
	}
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
	newGroup.materialData.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(newGroup.materialData.textureFilePath);

	// 頂点データを矩形で初期化
	std::vector<VertexData> rectangleVertices = {
		{ {  1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 右上
		{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 左上
		{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }, // 右下
		{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }, // 右下
		{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 左上
		{ { -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }  // 左下
	};

	// 頂点データを設定
	newGroup.vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * rectangleVertices.size());
	newGroup.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&newGroup.vertexData));
	std::memcpy(newGroup.vertexData, rectangleVertices.data(), sizeof(VertexData) * rectangleVertices.size());

	// 頂点バッファビューを設定
	newGroup.vertexBufferView.BufferLocation = newGroup.vertexResource->GetGPUVirtualAddress();
	newGroup.vertexBufferView.StrideInBytes = sizeof(VertexData);
	newGroup.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * rectangleVertices.size());


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
		Particle newParticle = MakeNewParticle(position);
		newParticle.transform.translate += Vector3(distribution(mt_), distribution(mt_), distribution(mt_));
		newParticle.velocity = Vector3(distribution(mt_), distribution(mt_), distribution(mt_));

		particleGroups_[groupName].particles.push_back(newParticle);
	}
}

void ParticleManager::EmitPlane(const std::string& groupName, const Vector3& position, uint32_t count)
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
		Particle newParticle = MakeNewPlaneParticle(position);
		//newParticle.transform.translate += Vector3(distribution(mt_), distribution(mt_), distribution(mt_));
		//newParticle.velocity = Vector3(distribution(mt_), distribution(mt_), distribution(mt_));
		particleGroups_[groupName].particles.push_back(newParticle);
	}
	//アンカーポイントを中心に設定
	
}

void ParticleManager::EmitRing(const std::string& groupName, const Vector3& position, uint32_t count)
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
		Particle newParticle = MakeNewParticle(position);
		particleGroups_[groupName].particles.push_back(newParticle);
	}
	//マテリアルデータをリングの形状に設定
	const uint32_t kRingDivide = 32;
	const float kOuterRadius = 1.0f;
	const float kInnerRadius = 0.2f;
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

	for (uint32_t i = 0; i < kRingDivide; ++i)
	{
		
	}

}

void ParticleManager::SetTexture(const std::string& groupName, const std::string& textureFilePath)
{
	//NOTE:エラーが出るときはファイルパスが間違っててテクスチャが読み込めていない可能性がある
	//パーティクルグループが存在するか確認
	if (particleGroups_.find(groupName) == particleGroups_.end())
	{
		// ログ出力
		Logger::Log("Particle group with name " + groupName + " does not exist.");
		assert(false);
	}
	//テクスチャを読み込む
	particleGroups_[groupName].materialData.textureFilePath = textureFilePath;
	TextureManager::GetInstance()->LoadTexture(particleGroups_[groupName].materialData.textureFilePath);
	//マテリアルデータにテクスチャのSRVインデックスを設定
	
	particleGroups_[groupName].materialData.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);
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

Particle ParticleManager::MakeNewParticle(const Vector3& position)
{
	Particle newParticle;
	newParticle.transform.translate = position;
	newParticle.transform.scale = { 1.0f, 1.0f, 1.0f };
	newParticle.transform.rotate = { 0.0f, 0.0f, 0.0f };
	newParticle.velocity = { 0.0f, 0.0f, 0.0f };
	newParticle.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	newParticle.lifeTime = 1.0f;
	newParticle.currentTime = 0.0f;
	return newParticle;
}

Particle ParticleManager::MakeNewPlaneParticle(const Vector3& position)
{
	Particle newParticle;
	newParticle.transform.translate = position;
	newParticle.transform.scale = { 0.05f, 1.0f, 1.0f };
	newParticle.transform.rotate = { 0.0f, 0.0f, 0.0f };
	newParticle.velocity = { 0.0f, 0.0f, 0.0f };
	newParticle.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	newParticle.lifeTime = 1.0f;
	newParticle.currentTime = 0.0f;
	return newParticle;
}

void ParticleManager::SetRotate(const std::string& groupName, const Vector3& rotation)
{//パーティクルグループが存在するか確認
	if (particleGroups_.find(groupName) == particleGroups_.end())
	{
		// ログ出力
		Logger::Log("Particle group with name " + groupName + " does not exist.");
		assert(false);
	}
	for (auto& particle : particleGroups_[groupName].particles)
	{
		particle.transform.rotate = rotation;
	}
}

void ParticleManager::SetRandomRotate(const std::string& groupName)
{
	//パーティクルグループが存在するか確認
	if (particleGroups_.find(groupName) == particleGroups_.end())
	{
		// ログ出力
		Logger::Log("Particle group with name " + groupName + " does not exist.");
		assert(false);
	}
	for (auto& particle : particleGroups_[groupName].particles)
	{
		std::uniform_real_distribution<float> distribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
		particle.transform.rotate = { 0.0f, 0.0f, distribution(mt_)};
	}
}

void ParticleManager::SetScale(const std::string& groupName, const Vector3& scale)
{//パーティクルグループが存在するか確認
	if (particleGroups_.find(groupName) == particleGroups_.end())
	{
		// ログ出力
		Logger::Log("Particle group with name " + groupName + " does not exist.");
		assert(false);
	}
	for (auto& particle : particleGroups_[groupName].particles)
	{
		particle.transform.scale = scale;
	}
}

void ParticleManager::SetRandomScale(const std::string& groupName)
{//パーティクルグループが存在するか確認
	if (particleGroups_.find(groupName) == particleGroups_.end())
	{
		// ログ出力
		Logger::Log("Particle group with name " + groupName + " does not exist.");
		assert(false);
	}
	for (auto& particle : particleGroups_[groupName].particles)
	{
		std::uniform_real_distribution<float> distribution(0.4f, 1.5f);
		particle.transform.scale = {0.05f, distribution(mt_), 1.0f };
	}
}

void ParticleManager::SetVelocity(const std::string& groupName, const Vector3& velocity)
{//パーティクルグループが存在するか確認
	if (particleGroups_.find(groupName) == particleGroups_.end())
	{
		// ログ出力
		Logger::Log("Particle group with name " + groupName + " does not exist.");
		assert(false);
	}
	for (auto& particle : particleGroups_[groupName].particles)
	{
		particle.velocity = velocity;
	}
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
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
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
