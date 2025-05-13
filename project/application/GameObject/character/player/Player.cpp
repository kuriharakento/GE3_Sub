#include "Player.h"

#include "application/GameObject/component/base/ICollisionComponent.h"

void Player::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager)
{
	Character::Initialize(object3dCommon, lightManager);
	//初期位置を設定
	gameObject_->SetPosition({ 0.0f, 1.0f, 0.0f });
}

void Player::Update()
{
	Character::Update();
}

void Player::Draw(CameraManager* camera)
{
	Character::Draw(camera);
}

void Player::AddComponent(const std::string& name, std::shared_ptr<IGameObjectComponent> comp)
{
	if (auto collider = std::dynamic_pointer_cast<ICollisionComponent>(comp))
	{
		//衝突判定コンポーネントの場合は、衝突時の処理を設定
		CollisionSettings(collider.get());
	}

	//コンポーネントを追加
	gameObject_->AddComponent(name, comp);
}

void Player::CollisionSettings(ICollisionComponent* collider)
{
	// 衝突時の処理を設定
	collider->SetOnEnter([this](GameObject* other) {
		
		});
	collider->SetOnStay([this](GameObject* other) {
		
		});
	collider->SetOnExit([this](GameObject* other) {
		
		});
}
