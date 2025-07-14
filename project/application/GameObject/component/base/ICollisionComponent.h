#pragma once
#include <functional>

#include "IGameObjectComponent.h"
#include "math/Vector3.h"

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

	// 前フレームの位置を設定。フレームの最初に行う
	void SetPreviousPosition(const Vector3& position) { previousPosition_ = position; }
	Vector3 GetPreviousPosition() const { return previousPosition_; }

	// スイープ判定を使用するかどうか
	void SetUseSweep(bool use) { useSweep_ = use; }
	bool UseSweep() const { return useSweep_; }

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
	// オーナーのポインタ
	GameObject* owner_ = nullptr;
	// 前フレームの位置
	Vector3 previousPosition_ = {};
	// スイープ判定を行うか
	bool useSweep_ = false;

private:
	CollisionCallback onEnter_ = nullptr;
	CollisionCallback onStay_ = nullptr;
	CollisionCallback onExit_ = nullptr;
};