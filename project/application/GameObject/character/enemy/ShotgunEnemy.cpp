#include "ShotgunEnemy.h"

#include "application/GameObject/component/action/ShotgunComponent.h"
#include "application/GameObject/component/collision/OBBColliderComponent.h"

void ShotgunEnemy::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, GameObject* target)
{
	EnemyBase::Initialize(object3dCommon, lightManager, target);
	// Shotgunのコンポーネントを追加
	AddComponent("ShotgunComponent", std::make_unique<ShotgunComponent>(object3dCommon, lightManager));
	// 衝突判定コンポーネントを追加
	AddComponent("OBBCollider", std::make_unique<OBBColliderComponent>(this));
}

void ShotgunEnemy::Update()
{
	EnemyBase::Update();
}

void ShotgunEnemy::Draw(CameraManager* camera)
{
	EnemyBase::Draw(camera);
}