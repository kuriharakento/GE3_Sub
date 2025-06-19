#include "AssaultEnemy.h"

#include "application/GameObject/component/action/AssaultEnemyBehavior.h"
#include "application/GameObject/component/action/AssaultRifleComponent.h"
#include "application/GameObject/component/collision/OBBColliderComponent.h"

void AssaultEnemy::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, GameObject* target)
{
	EnemyBase::Initialize(object3dCommon, lightManager, target);
	// AssaultRifleのコンポーネントを追加
	AddComponent("AssaultRifleComponent", std::make_unique<AssaultRifleComponent>(object3dCommon, lightManager));
	// ビヘイビアコンポーネントを追加
	AddComponent("AssaultEnemyBehavior", std::make_unique<AssaultEnemyBehavior>(target_));
	// 衝突判定コンポーネントを追加
	std::unique_ptr<OBBColliderComponent> collider = std::make_unique<OBBColliderComponent>(this);
	collider->SetOnEnter([this](GameObject* other) {
		if (other->GetTag() == "PlayerBullet")
		{
			isAlive_ = false; // プレイヤーの弾に当たったら死亡
		}
						 });
	collider->SetOnStay([this](GameObject* other) {
		// 衝突中の処理をここに記述
						});
	collider->SetOnExit([this](GameObject* other) {
		// 衝突終了時の処理をここに記述
						});
	AddComponent("OBBCollider",std::move(collider));
}

void AssaultEnemy::Update()
{
	EnemyBase::Update();
}

void AssaultEnemy::Draw(CameraManager* camera)
{
	EnemyBase::Draw(camera);
}