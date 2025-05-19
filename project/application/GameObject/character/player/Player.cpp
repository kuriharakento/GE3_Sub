#include "Player.h"

#include "application/GameObject/component/base/ICollisionComponent.h"
#include "base/Logger.h"

void Player::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager)
{
	Character::Initialize(object3dCommon, lightManager);
	//初期位置を設定
	transform_.translate = { 0.0f, 1.0f, 0.0f };
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
