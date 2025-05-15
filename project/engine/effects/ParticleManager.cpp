#include "ParticleManager.h"

#include <dxcapi.h>
#include <numbers>

#include "manager/TextureManager.h"
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
		// 各パーティクルグループのリソース解放
		for (auto& groupPair : instance_->particleGroups_)
		{
			ParticleGroup& group = groupPair.second;

			if (group.instancingResource)
			{
				group.instancingResource->Unmap(0, nullptr);
				group.instancingResource.Reset();
				group.instancingData = nullptr;
			}

			if (group.vertexResource)
			{
				//vertexResourceは変更するたびにUnmapしてるのでする必要なし
				group.vertexResource.Reset();
				group.vertexData = nullptr;
			}

			if (group.materialResource_)
			{
				group.materialResource_->Unmap(0, nullptr);
				group.materialResource_.Reset();
				group.materialData_ = nullptr;
			}
		}

		// パイプラインマネージャーの解放
		instance_->pipelineManager_.reset();
		
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

	//モデルデータの初期化
	InitializeModelData();

	//パイプラインマネージャーの初期化
	pipelineManager_ = std::make_unique<ParticlePipelineManager>();
	pipelineManager_->Initialize(dxCommon_);
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
		for (std::list<Particle>::iterator particleItr = group.particles.begin(); particleItr != group.particles.end();) {
			if (particleItr->lifeTime <= particleItr->currentTime) {
				particleItr = group.particles.erase(particleItr);
				continue;
			}

			// パーティクルの位置更新
			particleItr->transform.translate += particleItr->velocity * kDeltaTime;
			particleItr->currentTime += kDeltaTime;

			// パーティクルの透明度計算
			float alpha = 1.0f - (particleItr->currentTime / particleItr->lifeTime);
			alpha = std::clamp(alpha, 0.0f, 1.0f); // 0.0～1.0に制限
			particleItr->color.w = alpha;

			// インスタンス数の制限を守る
			if (group.instanceCount < kMaxParticleCount) {
				// スケール、回転、平行移動の行列を計算
				Matrix4x4 scaleMatrix = MakeScaleMatrix(particleItr->transform.scale);
				Matrix4x4 translateMatrix = MakeTranslateMatrix(particleItr->transform.translate);
				Matrix4x4 rotateMatrix = MakeRotateMatrix(particleItr->transform.rotate);

				// ビルボードが有効ならビルボード行列を適用
				Matrix4x4 worldMatrixInstancing = scaleMatrix * rotateMatrix;
				if (group.isBillboard) {
					worldMatrixInstancing = worldMatrixInstancing * billboardMatrix;
				}
				worldMatrixInstancing = worldMatrixInstancing * translateMatrix;

				// 世界行列とWVP行列の計算
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

	dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetRootSignature());

	/*--------------[ パイプラインステートの設定 ]-----------------*/

	dxCommon_->GetCommandList()->SetPipelineState(pipelineManager_->GetPipelineState());

	/*--------------[ プリミティブトポロジーの設定 ]-----------------*/

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& groupPair : particleGroups_)
	{
		ParticleGroup& group = groupPair.second;
		//頂点バッファビューの設定
		dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &group.vertexBufferView);
		//
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, group.materialResource_->GetGPUVirtualAddress());
		// インスタンシングデータのSRVのDescriptorTableの先頭を設定
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(group.instancingSrvIndex));
		// SRVのDescriptorTableの先頭を設定
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(group.materialData.textureIndex));
		// 頂点数を計算
		UINT vertexCount = group.vertexBufferView.SizeInBytes / group.vertexBufferView.StrideInBytes;
		// 描画
		dxCommon_->GetCommandList()->DrawInstanced(vertexCount, group.instanceCount, 0, 0);
	}
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
	newGroup.vertexResource->Unmap(0, nullptr);

	// 頂点バッファビューを設定
	newGroup.vertexBufferView.BufferLocation = newGroup.vertexResource->GetGPUVirtualAddress();
	newGroup.vertexBufferView.StrideInBytes = sizeof(VertexData);
	newGroup.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * rectangleVertices.size());

	//マテリアルデータを設定
	newGroup.materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));
	newGroup.materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&newGroup.materialData_));
	newGroup.materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	newGroup.materialData_->enableLighting = false;
	newGroup.materialData_->uvTransform = MakeIdentity4x4();

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
	// 頂点データをリングで初期化（triangle list）
	const uint32_t kRingDivide = 32;
	const float    kOuterRadius = 1.0f;
	const float    kInnerRadius = 0.2f;
	const float    radianPerDiv = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

	std::vector<VertexData> ringVertices;
	ringVertices.reserve(kRingDivide * 6);  // 1セグメントあたり6頂点

	for (uint32_t i = 0; i < kRingDivide; ++i)
	{
		// 0→1 の角度
		float theta0 = radianPerDiv * float(i);
		float theta1 = radianPerDiv * float(i + 1);

		// sin/cos をそれぞれ計算
		float c0 = std::cos(theta0), s0 = std::sin(theta0);
		float c1 = std::cos(theta1), s1 = std::sin(theta1);

		// テクスチャ座標 U
		float u0 = float(i) / float(kRingDivide);
		float u1 = float(i + 1) / float(kRingDivide);

		// ▽ 三角形 1： outer0 → outer1 → inner0
		ringVertices.push_back({ { c0 * kOuterRadius, s0 * kOuterRadius, 0, 1 }, { u0, 0 }, { 0,0,1 } });
		ringVertices.push_back({ { c1 * kOuterRadius, s1 * kOuterRadius, 0, 1 }, { u1, 0 }, { 0,0,1 } });
		ringVertices.push_back({ { c0 * kInnerRadius, s0 * kInnerRadius, 0, 1 }, { u0, 1 }, { 0,0,1 } });

		// ▽ 三角形 2： outer1 → inner1 → inner0
		ringVertices.push_back({ { c1 * kOuterRadius, s1 * kOuterRadius, 0, 1 }, { u1, 0 }, { 0,0,1 } });
		ringVertices.push_back({ { c1 * kInnerRadius, s1 * kInnerRadius, 0, 1 }, { u1, 1 }, { 0,0,1 } });
		ringVertices.push_back({ { c0 * kInnerRadius, s0 * kInnerRadius, 0, 1 }, { u0, 1 }, { 0,0,1 } });
	}
	// 頂点データを設定
	particleGroups_[groupName].vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * ringVertices.size());
	particleGroups_[groupName].vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroups_[groupName].vertexData));
	std::memcpy(particleGroups_[groupName].vertexData, ringVertices.data(), sizeof(VertexData) * ringVertices.size());
	particleGroups_[groupName].vertexResource->Unmap(0, nullptr);
	// 頂点バッファビューを設定
	particleGroups_[groupName].vertexBufferView.BufferLocation = particleGroups_[groupName].vertexResource->GetGPUVirtualAddress();
	particleGroups_[groupName].vertexBufferView.StrideInBytes = sizeof(VertexData);
	particleGroups_[groupName].vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * ringVertices.size());
}

void ParticleManager::EmitCylinder(const std::string& groupName, const Vector3& position, uint32_t count)
{
	//登録済みのパーティクルグループか確認
	if (particleGroups_.find(groupName) == particleGroups_.end())
	{
		// ログ出力
		Logger::Log("Particle group with name " + groupName + " does not exist.");
		assert(false);
	}
	//パーティクルを生成
	for (uint32_t i = 0; i < count; ++i)
	{
		Particle newParticle = MakeNewParticle(position);
		particleGroups_[groupName].particles.push_back(newParticle);
	}

	const uint32_t kCylinderDivide = 32; // 円柱の分割数
	const float kOuterRadius = 1.0f;     // 外側の半径
	const float kHeight = 2.0f;          // 円柱の高さ
	const float radianPerDiv = 2.0f * std::numbers::pi_v<float> / float(kCylinderDivide);

	std::vector<VertexData> cylinderVertices;

	//// 上面キャップの頂点データを生成
	//for (uint32_t index = 0; index < kCylinderDivide; ++index) {
	//	float sin0 = std::sin(radianPerDiv * index);
	//	float cos0 = std::cos(radianPerDiv * index);
	//	float sin1 = std::sin(radianPerDiv * (index + 1));
	//	float cos1 = std::cos(radianPerDiv * (index + 1));
	//	float u0 = float(index) / float(kCylinderDivide);
	//	float u1 = float(index + 1) / float(kCylinderDivide);

	//	// 三角形1: 中心 → 外側0 → 外側1
	//	cylinderVertices.push_back({ { 0.0f, kHeight / 2.0f, 0.0f, 1.0f }, { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } }); // 中心
	//	cylinderVertices.push_back({ { cos0 * kOuterRadius, kHeight / 2.0f, sin0 * kOuterRadius, 1.0f }, { u0, 0.0f }, { 0.0f, 1.0f, 0.0f } }); // 外側0
	//	cylinderVertices.push_back({ { cos1 * kOuterRadius, kHeight / 2.0f, sin1 * kOuterRadius, 1.0f }, { u1, 0.0f }, { 0.0f, 1.0f, 0.0f } }); // 外側1
	//}

	//// 下面キャップの頂点データを生成
	//for (uint32_t index = 0; index < kCylinderDivide; ++index) {
	//	float sin0 = std::sin(radianPerDiv * index);
	//	float cos0 = std::cos(radianPerDiv * index);
	//	float sin1 = std::sin(radianPerDiv * (index + 1));
	//	float cos1 = std::cos(radianPerDiv * (index + 1));
	//	float u0 = float(index) / float(kCylinderDivide);
	//	float u1 = float(index + 1) / float(kCylinderDivide);

	//	// 三角形1: 中心 → 外側1 → 外側0
	//	cylinderVertices.push_back({ { 0.0f, -kHeight / 2.0f, 0.0f, 1.0f }, { 0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f } }); // 中心
	//	cylinderVertices.push_back({ { cos1 * kOuterRadius, -kHeight / 2.0f, sin1 * kOuterRadius, 1.0f }, { u1, 0.0f }, { 0.0f, -1.0f, 0.0f } }); // 外側1
	//	cylinderVertices.push_back({ { cos0 * kOuterRadius, -kHeight / 2.0f, sin0 * kOuterRadius, 1.0f }, { u0, 0.0f }, { 0.0f, -1.0f, 0.0f } }); // 外側0
	//}

	// 側面の頂点データを生成
	for (uint32_t index = 0; index < kCylinderDivide; ++index) {
		float sin0 = std::sin(radianPerDiv * index);
		float cos0 = std::cos(radianPerDiv * index);
		float sin1 = std::sin(radianPerDiv * (index + 1));
		float cos1 = std::cos(radianPerDiv * (index + 1));
		float u0 = float(index) / float(kCylinderDivide);
		float u1 = float(index + 1) / float(kCylinderDivide);

		// 三角形1: 上外側0 → 上外側1 → 下外側0
		cylinderVertices.push_back({ { cos0 * kOuterRadius, kHeight / 2.0f, sin0 * kOuterRadius, 1.0f }, { u0, 0.0f }, { cos0, 0.0f, sin0 } });
		cylinderVertices.push_back({ { cos1 * kOuterRadius, kHeight / 2.0f, sin1 * kOuterRadius, 1.0f }, { u1, 0.0f }, { cos1, 0.0f, sin1 } });
		cylinderVertices.push_back({ { cos0 * kOuterRadius, -kHeight / 2.0f, sin0 * kOuterRadius, 1.0f }, { u0, 1.0f }, { cos0, 0.0f, sin0 } });

		// 三角形2: 上外側1 → 下外側1 → 下外側0
		cylinderVertices.push_back({ { cos1 * kOuterRadius, kHeight / 2.0f, sin1 * kOuterRadius, 1.0f }, { u1, 0.0f }, { cos1, 0.0f, sin1 } });
		cylinderVertices.push_back({ { cos1 * kOuterRadius, -kHeight / 2.0f, sin1 * kOuterRadius, 1.0f }, { u1, 1.0f }, { cos1, 0.0f, sin1 } });
		cylinderVertices.push_back({ { cos0 * kOuterRadius, -kHeight / 2.0f, sin0 * kOuterRadius, 1.0f }, { u0, 1.0f }, { cos0, 0.0f, sin0 } });
	}

	// 頂点データを設定
	particleGroups_[groupName].vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * cylinderVertices.size());
	particleGroups_[groupName].vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroups_[groupName].vertexData));
	std::memcpy(particleGroups_[groupName].vertexData, cylinderVertices.data(), sizeof(VertexData) * cylinderVertices.size());
	particleGroups_[groupName].vertexResource->Unmap(0, nullptr);

	particleGroups_[groupName].vertexBufferView.BufferLocation = particleGroups_[groupName].vertexResource->GetGPUVirtualAddress();
	particleGroups_[groupName].vertexBufferView.StrideInBytes = sizeof(VertexData);
	particleGroups_[groupName].vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * cylinderVertices.size());
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

Particle ParticleManager::MakeNewParticle(const Vector3& position)
{
	Particle newParticle;
	newParticle.transform.translate = position;
	newParticle.transform.scale = { 1.0f, 1.0f, 1.0f };
	newParticle.transform.rotate = { 0.0f, 0.0f, 0.0f };
	newParticle.velocity = { 0.0f, 0.0f, 0.0f };
	newParticle.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	newParticle.lifeTime = 10.0f;
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
	newParticle.lifeTime = 10.0f;
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
		particle.transform.rotate = { 0.0f, 0.0f, distribution(mt_) };
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
		particle.transform.scale = { 0.05f, distribution(mt_), 1.0f };
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

void ParticleManager::SetVertexData(const std::string& groupName, VertexShape shape)
{
	// グループが存在するか確認
	if (particleGroups_.find(groupName) == particleGroups_.end()) {
		Logger::Log("Particle group with name " + groupName + " does not exist.");
		assert(false);
	}

	std::vector<VertexData> vertices;

	switch (shape) {
	case VertexShape::Plane:
		// 平面の頂点データを生成（例として矩形と同じ）
		vertices = {
			{ {  1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
			{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
			{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
			{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
			{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
			{ { -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }
		};
		break;

	case VertexShape::Ring:
		const uint32_t kRingDivide = 32;
		const float kOuterRadius = 1.0f;
		const float kInnerRadius = 0.2f;
		const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

		for (uint32_t i = 0; i < kRingDivide; ++i) {
			float theta0 = radianPerDivide * i;
			float theta1 = radianPerDivide * (i + 1);

			float cos0 = std::cos(theta0);
			float sin0 = std::sin(theta0);
			float cos1 = std::cos(theta1);
			float sin1 = std::sin(theta1);

			float u0 = float(i) / float(kRingDivide);
			float u1 = float(i + 1) / float(kRingDivide);

			// 三角形1（outer0 → outer1 → inner0）
			vertices.push_back({ { cos0 * kOuterRadius, sin0 * kOuterRadius, 0.0f, 1.0f }, { u0, 0.0f }, { 0.0f, 0.0f, 1.0f } });
			vertices.push_back({ { cos1 * kOuterRadius, sin1 * kOuterRadius, 0.0f, 1.0f }, { u1, 0.0f }, { 0.0f, 0.0f, 1.0f } });
			vertices.push_back({ { cos0 * kInnerRadius, sin0 * kInnerRadius, 0.0f, 1.0f }, { u0, 1.0f }, { 0.0f, 0.0f, 1.0f } });

			// 三角形2（outer1 → inner1 → inner0）
			vertices.push_back({ { cos1 * kOuterRadius, sin1 * kOuterRadius, 0.0f, 1.0f }, { u1, 0.0f }, { 0.0f, 0.0f, 1.0f } });
			vertices.push_back({ { cos1 * kInnerRadius, sin1 * kInnerRadius, 0.0f, 1.0f }, { u1, 1.0f }, { 0.0f, 0.0f, 1.0f } });
			vertices.push_back({ { cos0 * kInnerRadius, sin0 * kInnerRadius, 0.0f, 1.0f }, { u0, 1.0f }, { 0.0f, 0.0f, 1.0f } });
		}
		break;
	}

	// 頂点データを設定
	ParticleGroup& group = particleGroups_[groupName];
	group.vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * vertices.size());
	group.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&group.vertexData));
	std::memcpy(group.vertexData, vertices.data(), sizeof(VertexData) * vertices.size());
	group.vertexResource->Unmap(0, nullptr);

	// 頂点バッファビューを設定
	group.vertexBufferView.BufferLocation = group.vertexResource->GetGPUVirtualAddress();
	group.vertexBufferView.StrideInBytes = sizeof(VertexData);
	group.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
}