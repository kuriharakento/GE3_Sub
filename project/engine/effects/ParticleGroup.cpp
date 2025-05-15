#include "ParticleGroup.h"

#include <algorithm>
#include <numbers>

#include "ParticleManager.h"
#include "manager/CameraManager.h"
#include "manager/SrvManager.h"
#include "base/DirectXCommon.h"
#include "base/Logger.h"
#include "manager/TextureManager.h"
#include "math/MathUtils.h"

ParticleGroup::~ParticleGroup()
{
	// リソースの解放
	if (instancingResource)
	{
		instancingResource->Unmap(0, nullptr);
		instancingResource.Reset();
		instancingData = nullptr;
	}
	if (vertexResource)
	{
		vertexResource.Reset();
		vertexData = nullptr;
	}
	if (materialResource_)
	{
		materialResource_->Unmap(0, nullptr);
		materialResource_.Reset();
		materialData_ = nullptr;
	}
	particles.clear();
	delete modelData_;
	modelData_ = nullptr;
}

void ParticleGroup::Initialize(const std::string& groupName, const std::string& textureFilePath)
{
	// 各種リソースの初期化
	// テクスチャの読み込み
	modelData_ = new MaterialData();
	modelData_->textureFilePath = textureFilePath;
	TextureManager::GetInstance()->LoadTexture(modelData_->textureFilePath);
	modelData_->textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_->textureFilePath);

	//マテリアルリソース
	materialResource_ = ParticleManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->enableLighting = false;

	// 頂点バッファの初期化
	std::vector<VertexData> rectangleVertices = {
		{ {  1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 右上
		{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 左上
		{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }, // 右下
		{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }, // 右下
		{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 左上
		{ { -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }  // 左下
	};
	vertexResource = ParticleManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * rectangleVertices.size());
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, rectangleVertices.data(), sizeof(VertexData) * rectangleVertices.size());
	vertexResource->Unmap(0, nullptr);

	// 頂点バッファービューの初期化
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * rectangleVertices.size());

	// インスタンシング用リソースの初期化
	instancingResource = ParticleManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(ParticleForGPU) * kMaxParticleCount);
	instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	instancingSrvIndex = ParticleManager::GetInstance()->GetSrvManager()->Allocate() + 1;
	// SRVの生成
	ParticleManager::GetInstance()->GetSrvManager()->CreateSRVforStructuredBuffer(
		instancingSrvIndex,
		instancingResource.Get(),
		kMaxParticleCount, // numElements: パーティクルの最大数
		sizeof(ParticleForGPU) // structureByteStride: 各パーティクルのサイズ
	);
}

void ParticleGroup::Update(CameraManager* camera)
{
	if (particles.empty()) { return; } // パーティクルがない場合は更新しない

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

    instanceCount = 0; // このグループのインスタンスカウントをリセット

    for (auto particleItr = particles.begin(); particleItr != particles.end(); )
    {
		// 寿命の更新
		if (UpdateLifeTime(particleItr))
		{
			// 寿命が切れたパーティクルを削除
			particleItr = particles.erase(particleItr);
			continue;
		}

        if (instanceCount < kMaxParticleCount)
        {
			// 座標を速度によって更新
			UpdateTranslate(particleItr);
			// インスタンスデータの更新
			UpdateInstanceData(*particleItr, billboardMatrix, camera);

            ++instanceCount;
        }
        ++particleItr;
    }
}


void ParticleGroup::Draw(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	// 頂点数を計算
	UINT vertexCount = static_cast<UINT>(vertexBufferView.SizeInBytes / vertexBufferView.StrideInBytes);
	// インスタンスがない場合は描画しない
	if (instanceCount == 0) { return; }

	//描画設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager->GetGPUDescriptorHandle(instancingSrvIndex));
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager->GetGPUDescriptorHandle(modelData_->textureIndex));
	// インスタンシング描画
	dxCommon->GetCommandList()->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void ParticleGroup::SetModelType(ParticleType type)
{
	switch (type)
	{
	case ParticleType::Plane:
		MakePlaneVertexData();
		break;
	case ParticleType::Ring:
		MakeRingVertexData();
		break;
	case ParticleType::Cylinder:
		MakeCylinderVertexData();
		break;
	default:
		Logger::Log("Invalid particle type.");
		assert(false);
		break;
	}
}

Vector3 ParticleGroup::GetUVTranslate() const
{
	return MathUtils::GetMatrixTranslate(materialData_->uvTransform);
}

Vector3 ParticleGroup::GetUVScale() const
{
	return MathUtils::GetMatrixScale(materialData_->uvTransform);
}

Vector3 ParticleGroup::GetUVRotate() const
{
	return MathUtils::GetMatrixRotate(materialData_->uvTransform);
}

void ParticleGroup::SetUVTranslate(const Vector3& translate)
{
	materialData_->uvTransform = MakeAffineMatrix(translate, GetUVRotate(), GetUVScale());
}

void ParticleGroup::SetUVScale(const Vector3& scale)
{
	materialData_->uvTransform = MakeAffineMatrix(GetUVTranslate(), GetUVRotate(), scale);
}

void ParticleGroup::SetUVRotate(const Vector3& rotate)
{
	materialData_->uvTransform = MakeAffineMatrix(GetUVTranslate(), rotate, GetUVScale());
}

void ParticleGroup::UpdateInstanceData(Particle& particle, const Matrix4x4& billboardMatrix, CameraManager* camera)
{
	// スケール、回転、平行移動の行列を計算
	Matrix4x4 scaleMatrix = MakeScaleMatrix(particle.transform.scale);
	Matrix4x4 rotateMatrix = MakeRotateMatrix(particle.transform.rotate);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(particle.transform.translate);
	// ビルボード適用
	Matrix4x4 worldMatrixInstancing = scaleMatrix * rotateMatrix;
	if (isBillboard_)
	{
		worldMatrixInstancing = worldMatrixInstancing * billboardMatrix;
	}
	worldMatrixInstancing = worldMatrixInstancing * translateMatrix;
	// WVP行列計算
	Matrix4x4 wvp = Multiply(worldMatrixInstancing,
							 Multiply(camera->GetActiveCamera()->GetViewMatrix(),
									  camera->GetActiveCamera()->GetProjectionMatrix()));
	// インスタンシング用データにセット
	if (instancingData)
	{
		instancingData[instanceCount].World = worldMatrixInstancing;
		instancingData[instanceCount].WVP = wvp;
		instancingData[instanceCount].color = particle.color;
	}
}

bool ParticleGroup::UpdateLifeTime(std::list<Particle>::iterator& itr)
{
	// 寿命を更新
	itr->currentTime += 1.0f / 60.0f;
	// 寿命が切れたらtrueを返す
	if (itr->currentTime >= itr->lifeTime)
	{
		return true;
	}
	return false;
}

void ParticleGroup::UpdateTranslate(std::list<Particle>::iterator& itr)
{
	// 速度を加算
	itr->transform.translate += itr->velocity * (1.0f / 60.0f); // 1フレーム分の時間を加算
}

void ParticleGroup::MakePlaneVertexData()
{
	// 頂点データを矩形で初期化
	std::vector<VertexData> rectangleVertices = {
	{ {  1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 右上
	{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 左上
	{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }, // 右下
	{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }, // 右下
	{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // 左上
	{ { -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }  // 左下
	};
	vertexResource = ParticleManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * rectangleVertices.size());
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, rectangleVertices.data(), sizeof(VertexData) * rectangleVertices.size());
	vertexResource->Unmap(0, nullptr);
	// 頂点バッファービューの再設定
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * rectangleVertices.size());
}

void ParticleGroup::MakeRingVertexData()
{
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
	vertexResource = ParticleManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * ringVertices.size());
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, ringVertices.data(), sizeof(VertexData) * ringVertices.size());
	vertexResource->Unmap(0, nullptr);

	// 頂点バッファービューの再設定
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * ringVertices.size());
}

void ParticleGroup::MakeCylinderVertexData()
{
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
	for (uint32_t index = 0; index < kCylinderDivide; ++index)
	{
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
	vertexResource = ParticleManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * cylinderVertices.size());
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, cylinderVertices.data(), sizeof(VertexData) * cylinderVertices.size());
	vertexResource->Unmap(0, nullptr);

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * cylinderVertices.size());
}
