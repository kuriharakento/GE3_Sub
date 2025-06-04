#include "AssaultEnemy.h"

#include "application/GameObject/component/action/AssaultRifleComponent.h"
#include "application/GameObject/component/collision/OBBColliderComponent.h"

void AssaultEnemy::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, GameObject* target)
{
	EnemyBase::Initialize(object3dCommon, lightManager, target);
	// AssaultRifleのコンポーネントを追加
	AddComponent("AssaultRifleComponent", std::make_unique<AssaultRifleComponent>(object3dCommon, lightManager));
	// 衝突判定コンポーネントを追加
	AddComponent("OBBCollider", std::make_unique<OBBColliderComponent>(this));
}

void AssaultEnemy::Update()
{
	EnemyBase::Update();
}

void AssaultEnemy::Draw(CameraManager* camera)
{
	EnemyBase::Draw(camera);
}