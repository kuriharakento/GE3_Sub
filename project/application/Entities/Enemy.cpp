#include "Enemy.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

void Enemy::Initialize(const std::string& filePath, Object3dCommon* objectCommon)
{
	//オブジェクトの初期化
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(objectCommon);
	object3d_->SetModel(filePath);

	//座標の初期化
	transform_ = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,10.0f }
	};
}

void Enemy::Update(CameraManager* camera)
{
#ifdef _DEBUG
	
#endif

	//行列の更新
	UpdateObjTransform(camera);
}

void Enemy::Draw()
{
	object3d_->Draw();
}

void Enemy::OnCollision(ICollidable* other)
{
	// 衝突時の処理を記述
	other->GetAttackPower();
}
