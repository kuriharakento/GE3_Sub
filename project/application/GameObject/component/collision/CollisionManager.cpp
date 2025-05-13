#include "CollisionManager.h"
#include <algorithm>

#include "math/AABB.h"
#include "application/GameObject/component/collision/AABBColliderComponent.h"
#include "application/GameObject/component/base/ICollisionComponent.h"

CollisionManager* CollisionManager::instance_ = nullptr; // シングルトンインスタンス

CollisionManager* CollisionManager::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new CollisionManager();
	}
	return instance_;
}

void CollisionManager::Register(ICollisionComponent* collider) {
    colliders_.push_back(collider);
}

void CollisionManager::Unregister(ICollisionComponent* collider) {
    colliders_.erase(std::remove(colliders_.begin(), colliders_.end(), collider), colliders_.end());
}

void CollisionManager::CheckCollisions()
 {
    std::unordered_set<CollisionPair, CollisionPairHash> newCollisions;

    for (size_t i = 0; i < colliders_.size(); ++i) {
        for (size_t j = i + 1; j < colliders_.size(); ++j) {
            ICollisionComponent* a = colliders_[i];
            ICollisionComponent* b = colliders_[j];

            bool isHit = false;

            // 衝突判定のディスパッチ
            ColliderType typeA = a->GetColliderType();
            ColliderType typeB = b->GetColliderType();

			//コライダーのタイプごとに衝突判定を行う
            if (typeA == ColliderType::AABB && typeB == ColliderType::AABB) {
				isHit = CheckCollision(static_cast<AABBColliderComponent*>(a), static_cast<AABBColliderComponent*>(b));
            }
            
			// 衝突している場合
            if (isHit) {
                CollisionPair pair{ a, b };
                newCollisions.insert(pair);
				//衝突した瞬間の処理
                if (!currentCollisions_.contains(pair)) {
                    a->CallOnEnter(b->GetOwner());
                    b->CallOnEnter(a->GetOwner());
                }
                else {
					//衝突している間の処理
                    a->CallOnStay(b->GetOwner());
                    b->CallOnStay(a->GetOwner());
                }
            }
        }
    }

    // 離れた衝突を処理
    for (const auto& pair : currentCollisions_) {
        if (!newCollisions.contains(pair)) {
            //衝突が離れた場合の処理
            pair.a->CallOnExit(pair.b->GetOwner());
            pair.b->CallOnExit(pair.a->GetOwner());
        }
    }

    currentCollisions_ = std::move(newCollisions);
}

bool CollisionManager::CheckCollision(const AABBColliderComponent* a, const AABBColliderComponent* b)
{
    const AABB& aBox = a->GetAABB();
    const AABB& bBox = b->GetAABB();

    return (aBox.max_.x >= bBox.min_.x && aBox.min_.x <= bBox.max_.x) &&
        (aBox.max_.y >= bBox.min_.y && aBox.min_.y <= bBox.max_.y) &&
        (aBox.max_.z >= bBox.min_.z && aBox.min_.z <= bBox.max_.z);
}
