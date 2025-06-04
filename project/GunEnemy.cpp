#include "GunEnemy.h"

#include "application/GameObject/component/action/FireComponent.h"
#include "application/GameObject/component/collision/OBBColliderComponent.h"

void GunEnemy::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, GameObject* target)
{
	EnemyBase::Initialize(object3dCommon, lightManager, target);

	//銃射撃のコンポーネントを追加
	AddComponent("FireComponent", std::make_shared<FireComponent>(object3dCommon, lightManager));

	// 衝突判定コンポーネントを追加
	AddComponent("OBBCollider", std::make_shared<OBBColliderComponent>(this));
}

void GunEnemy::Update()
{
	EnemyBase::Update();
}

void GunEnemy::Draw(CameraManager* camera)
{
	EnemyBase::Draw(camera);
}