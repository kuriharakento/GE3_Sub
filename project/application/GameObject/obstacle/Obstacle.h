#pragma once
#include "application/GameObject/base/GameObject.h"
#include "application/GameObject/component/base/ICollisionComponent.h"
#include "math/OBB.h"

class Obstacle : public GameObject
{
public:
	virtual ~Obstacle() = default;
	explicit Obstacle(const std::string& tag) : GameObject(tag) {}
	virtual void Initialize(Object3dCommon* object3dCommon, LightManager* lightManager);
	virtual void Update();
	virtual void Draw(CameraManager* camera);

	//トランスフォームの取得と設定
	const Vector3& GetPosition() const { return transform_.translate; }
	const Vector3& GetRotation() const { return transform_.rotate; }
	const Vector3& GetScale() const { return transform_.scale; }
	void SetPosition(const Vector3& position) { transform_.translate = position; }
	void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void AddComponent(const std::string& name, std::unique_ptr<IGameObjectComponent> comp);

protected:
	void CollisionSettings(ICollisionComponent* collider);
	void ResolvePenetration(GameObject* other);
};

