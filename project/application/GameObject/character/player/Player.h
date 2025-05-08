#pragma once
#include "application/GameObject/character/base/Character.h"

class Player : public Character
{
public:
	void Initialize(Object3dCommon* object3dCommon, LightManager* lightManager) override;
	void Update() override;
	void Draw(CameraManager* camera) override;

	GameObject* GetGameObject() { return gameObject_.get(); }

private:

};

