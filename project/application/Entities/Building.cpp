#include "Building.h"

#include "Player.h"

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
}

void Building::Update(CameraManager* camera)
{
	UpdateOBJTransform(camera);
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

void Building::OnCollision(ICollidable* other)
{
	//プレイヤーがぶつかった場合
	if (other->GetType() == ObjectType::Player)
	{
		// プレイヤーオブジェクトにキャスト
		Player* player = dynamic_cast<Player*>(other);
		if (player)
		{
			// プレイヤーの位置を取得
			Vector3 playerPos = player->GetPosition();
			
		}
	}
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
