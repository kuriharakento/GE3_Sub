#pragma once
#include <unordered_set>

#include "OBBColliderComponent.h"
#include "application/GameObject/component/base/ICollisionComponent.h"

class AABBColliderComponent;

class CollisionManager
{
public:
	static CollisionManager* GetInstance();
	void Initialize() { colliders_.clear(); }
    void Finalize() { colliders_.clear(), currentCollisions_.clear(); }

	void Register(ICollisionComponent* collider);
	void Unregister(ICollisionComponent* collider);
	void CheckCollisions();

private:
	static CollisionManager* instance_; // シングルトンインスタンス
    CollisionManager() = default;
    ~CollisionManager() = default;
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;

    // 衝突判定の具体的な関数群
    bool CheckCollision(const AABBColliderComponent* a, const AABBColliderComponent* b);
	bool CheckCollision(const OBBColliderComponent* a, const OBBColliderComponent* b);

    // ペアを識別するためのキー
    struct CollisionPair {
        const ICollisionComponent* a;
        const ICollisionComponent* b;

        bool operator==(const CollisionPair& other) const {
            return (a == other.a && b == other.b) || (a == other.b && b == other.a);
        }
    };

    struct CollisionPairHash {
        std::size_t operator()(const CollisionPair& pair) const {
            return std::hash<const void*>()(pair.a) ^ std::hash<const void*>()(pair.b);
        }
    };

    std::vector<ICollisionComponent*> colliders_;

    // 現在接触しているペア
    std::unordered_set<CollisionPair, CollisionPairHash> currentCollisions_;

};

