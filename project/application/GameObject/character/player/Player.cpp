#include "Player.h"

#include "application/GameObject/component/action/AssaultRifleComponent.h"
#include "application/GameObject/component/action/PistolComponent.h"
#include "application/GameObject/component/action/MoveComponent.h"
#include "application/GameObject/component/base/ICollisionComponent.h"
#include "application/GameObject/component/collision/OBBColliderComponent.h"
#include "base/Logger.h"

void Player::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager)
{
	Character::Initialize(object3dCommon, lightManager);
	//初期位置を設定
	transform_.translate = { 0.0f, 1.0f, 0.0f };

	// 移動コンポーネントを追加
	AddComponent("MoveComponent", std::make_unique<MoveComponent>(5.0f));
	// 射撃コンポーネントを追加
	AddComponent("PistolComponent", std::make_unique<AssaultRifleComponent>(object3dCommon, lightManager));
	// 衝突判定コンポーネント
	AddComponent("OBBCollider", std::make_unique<OBBColliderComponent>(this));

}

void Player::Update()
{
	Character::Update();
}

void Player::Draw(CameraManager* camera)
{
	Character::Draw(camera);
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
