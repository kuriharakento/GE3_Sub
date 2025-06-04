#include "PistolEnemy.h"

#include "application/GameObject/component/action/PistolComponent.h"
#include "application/GameObject/component/action/ShotgunComponent.h"
#include "application/GameObject/component/collision/OBBColliderComponent.h"

void PistolEnemy::Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, GameObject* target)
{
	EnemyBase::Initialize(object3dCommon, lightManager, target);

	//銃射撃のコンポーネントを追加
	AddComponent("PistolComponent", std::make_unique<PistolComponent>(object3dCommon, lightManager));

	// 衝突判定コンポーネントを追加
	AddComponent("OBBCollider", std::make_unique<OBBColliderComponent>(this));
}

void PistolEnemy::Update()
{
	EnemyBase::Update();
}

void PistolEnemy::Draw(CameraManager* camera)
{
	EnemyBase::Draw(camera);
}