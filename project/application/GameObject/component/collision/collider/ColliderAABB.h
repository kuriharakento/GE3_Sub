#pragma once
#include "ICollider.h"

class ColliderAABB : public ICollider
{
public:
	ColliderAABB() = default;

	ColliderType GetColliderType() const override {
		return ColliderType::AABB;
	}
	bool CheckCollision(const ICollider* other) const override;

	bool CheckCollisionWithAABB(const ColliderAABB* other) const override;

	bool CheckCollisionWithOBB(const ColliderOBB* other) const override;

	void SetPosition(const Vector3& pos) override { position_ = pos; }
	void SetScale(const Vector3& scale) override { scale_ = scale; }
	const Vector3& GetPosition() const override { return position_; }
	const Vector3& GetScale() const override { return scale_; }

private:
	Vector3 position_; // コリジョンの位置
	Vector3 scale_;    // コリジョンのスケール
};
