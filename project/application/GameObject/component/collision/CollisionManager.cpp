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
			/* AABB vs AABB */
            if (typeA == ColliderType::AABB && typeB == ColliderType::AABB) {
				isHit = CheckCollision(static_cast<AABBColliderComponent*>(a), static_cast<AABBColliderComponent*>(b));
            }
            /* OBB vs OBB */
            else if (typeA == ColliderType::OBB && typeB == ColliderType::OBB) {
                isHit = CheckCollision(static_cast<OBBColliderComponent*>(a), static_cast<OBBColliderComponent*>(b));
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

bool CollisionManager::CheckCollision(const OBBColliderComponent* a, const OBBColliderComponent* b)
{
    const OBB& obbA = a->GetOBB();
    const OBB& obbB = b->GetOBB();

    // OBBの回転行列（各軸ベクトル）
    Matrix4x4 rotA = obbA.rotate;
    Matrix4x4 rotB = obbB.rotate;

    Vector3 axesA[3] = {
        Vector3::Normalize(Vector3(rotA.m[0][0], rotA.m[0][1], rotA.m[0][2])),
        Vector3::Normalize(Vector3(rotA.m[1][0], rotA.m[1][1], rotA.m[1][2])),
        Vector3::Normalize(Vector3(rotA.m[2][0], rotA.m[2][1], rotA.m[2][2]))
    };

    Vector3 axesB[3] = {
        Vector3::Normalize(Vector3(rotB.m[0][0], rotB.m[0][1], rotB.m[0][2])),
        Vector3::Normalize(Vector3(rotB.m[1][0], rotB.m[1][1], rotB.m[1][2])),
        Vector3::Normalize(Vector3(rotB.m[2][0], rotB.m[2][1], rotB.m[2][2]))
    };

    // 15の分離軸
    Vector3 testAxes[15];
    int axisCount = 0;

    // OBB A, Bのローカル軸
    for (int i = 0; i < 3; ++i) testAxes[axisCount++] = axesA[i];
    for (int i = 0; i < 3; ++i) testAxes[axisCount++] = axesB[i];

    // クロス積
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            testAxes[axisCount++] = Vector3::Normalize(Vector3::Cross(axesA[i], axesB[j]));
        }
    }

    Vector3 toCenter = obbB.center - obbA.center;

    for (int i = 0; i < 15; ++i) {
        const Vector3& axis = testAxes[i];
        if (axis.x == 0 && axis.y == 0 && axis.z == 0) continue; // 無効軸スキップ

        // 射影幅
        float aProj =
            std::abs(Vector3::Dot(axesA[0] * obbA.size.x, axis)) +
            std::abs(Vector3::Dot(axesA[1] * obbA.size.y , axis)) +
            std::abs(Vector3::Dot(axesA[2] * obbA.size.z, axis));

        float bProj =
            std::abs(Vector3::Dot(axesB[0] * obbB.size.x, axis)) +
            std::abs(Vector3::Dot(axesB[1] * obbB.size.y, axis)) +
            std::abs(Vector3::Dot(axesB[2] * obbB.size.z, axis));

        float distance = std::abs(Vector3::Dot(toCenter, axis));

        if (distance > aProj + bProj) {
            // 非衝突
            return false;
        }
    }
	// 衝突
    return true;
}
