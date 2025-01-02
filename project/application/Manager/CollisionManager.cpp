#include "CollisionManager.h"

void CollisionManager::AddCollidable(ICollidable* collidable) {
    collidables_.push_back(collidable);
}

void CollisionManager::Initialize()
{
	// すべてクリア
	collidables_.clear();
}

void CollisionManager::Update() {
    // リスト内のすべてのコライダブルオブジェクトをペアでチェック
    for (auto itrA = collidables_.begin(); itrA != collidables_.end(); ++itrA) {
        ICollidable* collidableA = *itrA;

        // 現在の要素以降を対象にペアを作成
        for (auto itrB = std::next(itrA); itrB != collidables_.end(); ++itrB) {
            ICollidable* collidableB = *itrB;

            // 衝突判定
            if (CheckCollision(collidableA, collidableB)) {
                // 双方向で衝突時の処理を呼び出す
                collidableA->OnCollision(collidableB);
                collidableB->OnCollision(collidableA);
            }
        }
    }
}

void CollisionManager::Clear()
{
	// すべてクリア
	collidables_.clear();
}

bool CollisionManager::CheckCollision(ICollidable* a, ICollidable* b) {
	if (a->GetType() == b->GetType()) return false;

    const AABB& boxA = a->GetBoundingBox();
    const AABB& boxB = b->GetBoundingBox();

    // オブジェクトの位置を取得
    const Vector3& posA = a->GetPosition();
    const Vector3& posB = b->GetPosition();

    // 各軸での重なりをチェック
    bool overlapX = (posA.x + boxA.max.x >= posB.x + boxB.min.x) && (posA.x + boxA.min.x <= posB.x + boxB.max.x);
    bool overlapY = (posA.y + boxA.max.y >= posB.y + boxB.min.y) && (posA.y + boxA.min.y <= posB.y + boxB.max.y);
    bool overlapZ = (posA.z + boxA.max.z >= posB.z + boxB.min.z) && (posA.z + boxA.min.z <= posB.z + boxB.max.z);

    return overlapX && overlapY && overlapZ;
}

