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
				if ((collidableA->GetType() == ObjectType::Player && collidableB->GetType() == ObjectType::Building) || (collidableB->GetType() == ObjectType::Player && collidableA->GetType() == ObjectType::Building))
				{
					HandlePlayerBuildingCollision(collidableA, collidableB);
				} else if ((collidableA->GetType() == ObjectType::Enemy && collidableB->GetType() == ObjectType::Building) || (collidableB->GetType() == ObjectType::Enemy && collidableA->GetType() == ObjectType::Building))
				{
					HandleEnemyBuildingCollision(collidableA, collidableB);
				}else
				{
					// 双方向で衝突時の処理を呼び出す
					collidableA->OnCollision(collidableB);
					collidableB->OnCollision(collidableA);
				}
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

void CollisionManager::HandlePlayerBuildingCollision(ICollidable* a, ICollidable* b)
{
	ICollidable* player = nullptr;
	ICollidable* building = nullptr;

	// プレイヤーと建物のペアを特定
	if (a->GetType() == ObjectType::Player && b->GetType() == ObjectType::Building) {
		player = a;
		building = b;
	} else if (b->GetType() == ObjectType::Player && a->GetType() == ObjectType::Building) {
		player = b;
		building = a;
	} else {
		// プレイヤーと建物の組み合わせ以外は処理しない
		return;
	}

	// プレイヤーと建物のAABBを取得
	const AABB& playerBox = player->GetBoundingBox();
	const AABB& buildingBox = building->GetBoundingBox();

	// プレイヤーと建物の位置
	Vector3 playerPos = player->GetPosition();
	const Vector3& buildingPos = building->GetPosition();

	// 重なり量を計算
	float overlapX = std::min(playerPos.x + playerBox.max.x, buildingPos.x + buildingBox.max.x) -
		std::max(playerPos.x + playerBox.min.x, buildingPos.x + buildingBox.min.x);

	float overlapY = std::min(playerPos.y + playerBox.max.y, buildingPos.y + buildingBox.max.y) -
		std::max(playerPos.y + playerBox.min.y, buildingPos.y + buildingBox.min.y);

	float overlapZ = std::min(playerPos.z + playerBox.max.z, buildingPos.z + buildingBox.max.z) -
		std::max(playerPos.z + playerBox.min.z, buildingPos.z + buildingBox.min.z);

	// 重なりが小さい方向に押し戻す
	if (overlapX < overlapY && overlapX < overlapZ) {
		// X方向
		if (playerPos.x < buildingPos.x) {
			playerPos.x -= overlapX;
		} else {
			playerPos.x += overlapX;
		}
	} else if (overlapY < overlapX && overlapY < overlapZ) {
		// Y方向
		if (playerPos.y < buildingPos.y) {
			playerPos.y -= overlapY;
		} else {
			playerPos.y += overlapY;
		}
	} else {
		// Z方向
		if (playerPos.z < buildingPos.z) {
			playerPos.z -= overlapZ;
		} else {
			playerPos.z += overlapZ;
		}
	}

	// プレイヤーの位置を更新
	player->SetPosition(playerPos);
}

void CollisionManager::HandleEnemyBuildingCollision(ICollidable* a, ICollidable* b)
{
	ICollidable* enemy = nullptr;
	ICollidable* building = nullptr;

	// プレイヤーと建物のペアを特定
	if (a->GetType() == ObjectType::Enemy && b->GetType() == ObjectType::Building) {
		enemy = a;
		building = b;
	} else if (b->GetType() == ObjectType::Enemy && a->GetType() == ObjectType::Building) {
		enemy = b;
		building = a;
	} else {
		// プレイヤーと建物の組み合わせ以外は処理しない
		return;
	}

	// プレイヤーと建物のAABBを取得
	const AABB& playerBox = enemy->GetBoundingBox();
	const AABB& buildingBox = building->GetBoundingBox();

	// プレイヤーと建物の位置
	Vector3 enemyPos = enemy->GetPosition();
	const Vector3& buildingPos = building->GetPosition();

	// 重なり量を計算
	float overlapX = std::min(enemyPos.x + playerBox.max.x, buildingPos.x + buildingBox.max.x) -
		std::max(enemyPos.x + playerBox.min.x, buildingPos.x + buildingBox.min.x);

	float overlapY = std::min(enemyPos.y + playerBox.max.y, buildingPos.y + buildingBox.max.y) -
		std::max(enemyPos.y + playerBox.min.y, buildingPos.y + buildingBox.min.y);

	float overlapZ = std::min(enemyPos.z + playerBox.max.z, buildingPos.z + buildingBox.max.z) -
		std::max(enemyPos.z + playerBox.min.z, buildingPos.z + buildingBox.min.z);

	// 重なりが小さい方向に押し戻す
	if (overlapX < overlapY && overlapX < overlapZ) {
		// X方向
		if (enemyPos.x < buildingPos.x) {
			enemyPos.x -= overlapX;
		} else {
			enemyPos.x += overlapX;
		}
	} else if (overlapY < overlapX && overlapY < overlapZ) {
		// Y方向
		if (enemyPos.y < buildingPos.y) {
			enemyPos.y -= overlapY;
		} else {
			enemyPos.y += overlapY;
		}
	} else {
		// Z方向
		if (enemyPos.z < buildingPos.z) {
			enemyPos.z -= overlapZ;
		} else {
			enemyPos.z += overlapZ;
		}
	}

	enemy->SetPosition(enemyPos);
}

