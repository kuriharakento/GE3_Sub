#include "ColliderAABB.h"

bool ColliderAABB::CheckCollision(const ICollider* other) const
{
	if (other->GetColliderType() == ColliderType::AABB) {
		return CheckCollisionWithAABB(static_cast<const ColliderAABB*>(other));
	}
	/*else if (other->GetColliderType() == ColliderType::OBB) {
		return CheckCollisionWithOBB(static_cast<const ColliderOBB*>(other));
	}*/
	// 他のコリジョンタイプに対する処理を追加することも可能
	return false;
}

bool ColliderAABB::CheckCollisionWithAABB(const ColliderAABB* other) const
{
    Vector3 minA = position_ - scale_ * 0.5f;
    Vector3 maxA = position_ + scale_ * 0.5f;

    Vector3 minB = other->position_ - other->scale_ * 0.5f;
    Vector3 maxB = other->position_ + other->scale_ * 0.5f;

    return (minA.x <= maxB.x && maxA.x >= minB.x) &&
        (minA.y <= maxB.y && maxA.y >= minB.y) &&
        (minA.z <= maxB.z && maxA.z >= minB.z);
}

bool ColliderAABB::CheckCollisionWithOBB(const ColliderOBB* other) const
{
	return true;
}
