#pragma once
#include "application/GameObject/character/enemy/base/EnemyBase.h"

class GunEnemy : public EnemyBase
{
public:
	GunEnemy() : Character("GunEnemy") {}
	void Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, GameObject* target) override;
	void Update() override;
	void Draw(CameraManager* camera) override;
private:
};

