#pragma once

#include "application/GameObject/component/base/ICollisionComponent.h"
#include "math/AABB.h"
#include "Math/Vector3.h"

class AABBColliderComponent : public ICollisionComponent
{
public:
	AABBColliderComponent(GameObject* owner);

	void Update(GameObject* owner) override;
	ColliderType GetColliderType() const override { return ColliderType::AABB; }
	const AABB& GetAABB() const { return aabb_; }

private:
	AABB aabb_;				// AABB
};
