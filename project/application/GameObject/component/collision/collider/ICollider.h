#pragma once
#include <functional>

#include "application/GameObject/base/GameObject.h"

class ICollider
{
public:
	//当たり判定の種類
	enum class ColliderType
	{
		Sphere,
		AABB,
		OBB,
		Capsule,
	};

	using CollisionCallbackFunc = std::function<void(GameObject* other)>;
	struct CollisionCallback
	{
		CollisionCallbackFunc onEnter = nullptr;
		CollisionCallbackFunc onStay = nullptr;
		CollisionCallbackFunc onExit = nullptr;
	};

	virtual ~ICollider() = default;

	virtual bool CheckCollision(const ICollider* other) const = 0;
	virtual bool CheckCollisionWithAABB(const class ColliderAABB* other) const = 0;
	virtual bool CheckCollisionWithOBB(const class ColliderOBB* other) const = 0;

	virtual ColliderType GetColliderType() const = 0;

	//アクセッサ
	virtual const Vector3& GetPosition() const = 0;
	virtual const Vector3& GetScale() const = 0;
	virtual void SetPosition(const Vector3& pos) = 0;
	virtual void SetScale(const Vector3& scale) = 0;

protected:
	CollisionCallback callback_;
};
