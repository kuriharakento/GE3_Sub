#pragma once
#include "application/GameObject/component/base/ICollisionComponent.h"
#include "math/OBB.h"

class OBBColliderComponent : public ICollisionComponent
{
public:
	OBBColliderComponent(GameObject* owner);
	~OBBColliderComponent() override = default;
	void Update(GameObject* owner) override;
	ColliderType GetColliderType() const override { return ColliderType::OBB; }
	OBB GetOBB() const { return obb_; }

private:
	OBB obb_;				// OBB
};

