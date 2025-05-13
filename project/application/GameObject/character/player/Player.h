#pragma once
#include "application/GameObject/character/base/Character.h"
#include "application/GameObject/component/base/ICollisionComponent.h"

class Player : public Character
{
public:
	Player(std::string tag) : Character(tag) {}
	void Initialize(Object3dCommon* object3dCommon, LightManager* lightManager) override;
	void Update() override;
	void Draw(CameraManager* camera) override;

	void AddComponent(const std::string& name, std::shared_ptr<IGameObjectComponent> comp);

private:
	void CollisionSettings(ICollisionComponent* collider);
};

