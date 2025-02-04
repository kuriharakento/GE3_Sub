#include "Building.h"

#include "Player.h"
#include "audio/Audio.h"

Building::Building()
{
	transform_ = {
			{ 1.0f,1.0f,1.0f },
			{ 0.0f,0.0f,0.0f },
			{ 0.0f,0.0f,10.0f }
	};
	hitBox_.min = Vector3(-0.5f, -0.5f, -0.5f);
	hitBox_.max = Vector3(0.5f, 0.5f, 0.5f);
	type_ = ObjectType::Building;
	status_.health = 100.0f;
	status_.attackPower = 0.0f;
	status_.speed = 0.0f;
	status_.isAlive = true;
}

void Building::Initialize(const std::string& filePath, Object3dCommon* objectCommon)
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

	//ステータスの初期化
	status_.isAlive = true;
	status_.health = 100.0f * (object3d_->GetScale().Length() * 3.0f);
	status_.attackPower = 0.0f;
	status_.speed = 0.0f;
}

void Building::Update(CameraManager* camera)
{
	if(status_.health <= 0.0f)
	{
		status_.isAlive = false;
		Audio::GetInstance()->PlayWave("explosion", false);
	}
	//行列の更新
	UpdateOBJTransform(camera);
	//AABBの更新
	UpdateAABB();
}

void Building::Draw()
{
	object3d_->Draw();
}

const AABB& Building::GetBoundingBox() const
{
	return hitBox_;
}

Vector3 Building::GetPosition() const
{
	return transform_.translate;
}

void Building::SetPosition(const Vector3& position)
{
	transform_.translate = position;
}

void Building::UpdateAABB()
{
	hitBox_.min = -transform_.scale;
	hitBox_.max = transform_.scale;
}

void Building::OnCollision(ICollidable* other)
{
	//プレイヤーが撃った弾ならダメージを受けない
	status_.health -= other->GetAttackPower();
}

ObjectType Building::GetType() const
{
	return type_;
}

void Building::UpdateOBJTransform(CameraManager* camera)
{
	object3d_->SetTranslate(transform_.translate);
	object3d_->SetRotate(transform_.rotate);
	object3d_->SetScale(transform_.scale);
	object3d_->Update(camera);
}
