#pragma once
#include <functional>

#include "IGameObjectComponent.h"

class CollisionManager;

//当たり判定の種類
enum class ColliderType
{
	AABB,
	Sphere,
	OBB,
};

class ICollisionComponent : public virtual IGameObjectComponent
{
public:
	virtual ~ICollisionComponent();
	ICollisionComponent(GameObject* owner);

	using CollisionCallback = std::function<void(GameObject* other)>;

	virtual ColliderType GetColliderType() const = 0;

	//コールバック設定
	void SetOnEnter(CollisionCallback callback) { onEnter_ = callback; }
	void SetOnStay(CollisionCallback callback) { onStay_ = callback; }
	void SetOnExit(CollisionCallback callback) { onExit_ = callback; }

	//コールバック実行
	void CallOnEnter(GameObject* other) const { if (onEnter_) onEnter_(other); }
	void CallOnStay(GameObject* other) const { if (onStay_) onStay_(other); }
	void CallOnExit(GameObject* other) const { if (onExit_) onExit_(other); }

	GameObject* GetOwner() const { return owner_; }

protected:
	GameObject* owner_ = nullptr;

private:
	CollisionCallback onEnter_ = nullptr;
	CollisionCallback onStay_ = nullptr;
	CollisionCallback onExit_ = nullptr;
};