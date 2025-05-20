#include "CollisionManager.h"
#include <algorithm>

#include "math/AABB.h"
#include "application/GameObject/component/collision/AABBColliderComponent.h"
#include "application/GameObject/component/base/ICollisionComponent.h"
#include "application/GameObject/base/GameObject.h"
#include "base/Logger.h"

CollisionManager* CollisionManager::instance_ = nullptr; // シングルトンインスタンス

CollisionManager* CollisionManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new CollisionManager();
	}
	return instance_;
}

void CollisionManager::Register(ICollisionComponent* collider)
{
	colliders_.push_back(collider);
}

void CollisionManager::Unregister(ICollisionComponent* collider)
{
	colliders_.erase(std::remove(colliders_.begin(), colliders_.end(), collider), colliders_.end());

	// currentCollisions_ から該当コライダーを含むペアを削除
	for (auto it = currentCollisions_.begin(); it != currentCollisions_.end(); )
	{
		if (it->a == collider || it->b == collider)
		{
			it = currentCollisions_.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CollisionManager::CheckCollisions()
{
	std::unordered_set<CollisionPair, CollisionPairHash> newCollisions;

	for (size_t i = 0; i < colliders_.size(); ++i)
	{
		for (size_t j = i + 1; j < colliders_.size(); ++j)
		{
			ICollisionComponent* a = colliders_[i];
			ICollisionComponent* b = colliders_[j];

			bool isHit = false;

			// 衝突判定のディスパッチ
			ColliderType typeA = a->GetColliderType();
			ColliderType typeB = b->GetColliderType();

			//コライダーのタイプごとに衝突判定を行う
			/* AABB vs AABB */
			if (typeA == ColliderType::AABB && typeB == ColliderType::AABB)
			{
				isHit = CheckCollision(static_cast<AABBColliderComponent*>(a), static_cast<AABBColliderComponent*>(b));
			}
			/* OBB vs OBB */
			else if (typeA == ColliderType::OBB && typeB == ColliderType::OBB)
			{
				isHit = CheckCollision(static_cast<OBBColliderComponent*>(a), static_cast<OBBColliderComponent*>(b));
			}
			/* AABB vs OBB */
			else if (typeA == ColliderType::AABB && typeB == ColliderType::OBB)
			{
				isHit = CheckCollision(static_cast<AABBColliderComponent*>(a), static_cast<OBBColliderComponent*>(b));
			}
			else if (typeA == ColliderType::OBB && typeB == ColliderType::AABB)
			{
				isHit = CheckCollision(static_cast<AABBColliderComponent*>(b), static_cast<OBBColliderComponent*>(a));
			}

			// 衝突している場合
			if (isHit)
			{
				CollisionPair pair{ a, b };
				newCollisions.insert(pair);
				//衝突した瞬間の処理
				if (!currentCollisions_.contains(pair))
				{
					a->CallOnEnter(b->GetOwner());
					b->CallOnEnter(a->GetOwner());
					//ログで確認できるように表示
					LogCollision("Enter", a, b);
				}
				else
				{
					//衝突している間の処理
					a->CallOnStay(b->GetOwner());
					b->CallOnStay(a->GetOwner());
					//ログで確認できるように表示
					LogCollision("Stay", a, b);
				}
			}
		}
	}

	// 離れた衝突を処理
	for (const auto& pair : currentCollisions_)
	{
		if (!newCollisions.contains(pair))
		{
			//衝突が離れた場合の処理
			pair.a->CallOnExit(pair.b->GetOwner());
			pair.b->CallOnExit(pair.a->GetOwner());
			//ログで確認できるように表示
			LogCollision("Exit", pair.a, pair.b);
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

	Vector3 axesA[3] =
	{
		Vector3::Normalize(Vector3(rotA.m[0][0], rotA.m[0][1], rotA.m[0][2])),
		Vector3::Normalize(Vector3(rotA.m[1][0], rotA.m[1][1], rotA.m[1][2])),
		Vector3::Normalize(Vector3(rotA.m[2][0], rotA.m[2][1], rotA.m[2][2]))
	};

	Vector3 axesB[3] =
	{
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
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			testAxes[axisCount++] = Vector3::Normalize(Vector3::Cross(axesA[i], axesB[j]));
		}
	}

	Vector3 toCenter = obbB.center - obbA.center;

	for (int i = 0; i < 15; ++i)
	{
		const Vector3& axis = testAxes[i];
		if (axis.x == 0 && axis.y == 0 && axis.z == 0) continue; // 無効軸スキップ

		// 射影幅
		float aProj =
			std::abs(Vector3::Dot(axesA[0] * obbA.size.x, axis)) +
			std::abs(Vector3::Dot(axesA[1] * obbA.size.y, axis)) +
			std::abs(Vector3::Dot(axesA[2] * obbA.size.z, axis));

		float bProj =
			std::abs(Vector3::Dot(axesB[0] * obbB.size.x, axis)) +
			std::abs(Vector3::Dot(axesB[1] * obbB.size.y, axis)) +
			std::abs(Vector3::Dot(axesB[2] * obbB.size.z, axis));

		float distance = std::abs(Vector3::Dot(toCenter, axis));

		if (distance > aProj + bProj)
		{
			// 非衝突
			return false;
		}
	}
	// 衝突
	return true;
}

bool CollisionManager::CheckCollision(const AABBColliderComponent* a, const OBBColliderComponent* b)
{
	const AABB& aBox = a->GetAABB();
	const OBB& obb = b->GetOBB();

	// OBBの回転行列（各軸ベクトル）
	Matrix4x4 rot = obb.rotate;

	// OBBの軸ベクトル
	Vector3 axes[3] =
	{
		Vector3::Normalize(Vector3(rot.m[0][0], rot.m[0][1], rot.m[0][2])),
		Vector3::Normalize(Vector3(rot.m[1][0], rot.m[1][1], rot.m[1][2])),
		Vector3::Normalize(Vector3(rot.m[2][0], rot.m[2][1], rot.m[2][2]))
	};

	// AABBの中心座標をOBBのローカル空間に変換
	Vector3 toCenter = aBox.GetCenter() - obb.center;

	// AABBの半分のサイズ
	Vector3 aHalfSize = aBox.GetHalfSize();

	// 15の分離軸
	Vector3 testAxes[6];
	int axisCount = 0;

	// OBBの軸をテスト軸に追加
	for (int i = 0; i < 3; ++i) testAxes[axisCount++] = axes[i];

	// AABBの軸（X, Y, Z）をテスト軸に追加
	testAxes[axisCount++] = Vector3(1, 0, 0);
	testAxes[axisCount++] = Vector3(0, 1, 0);
	testAxes[axisCount++] = Vector3(0, 0, 1);

	// 各軸で分離軸テストを実行
	for (int i = 0; i < axisCount; ++i)
	{
		const Vector3& axis = testAxes[i];

		// AABBの射影幅
		float aProj = std::abs(Vector3::Dot(axis, Vector3(aHalfSize.x, 0.0f, 0.0f))) +
			std::abs(Vector3::Dot(axis, Vector3(0.0f, aHalfSize.y, 0.0f))) +
			std::abs(Vector3::Dot(axis, Vector3(0.0f, 0.0f, aHalfSize.z)));

		// OBBの射影幅
		float bProj = std::abs(Vector3::Dot(axes[0] * obb.size.x, axis)) +
			std::abs(Vector3::Dot(axes[1] * obb.size.y, axis)) +
			std::abs(Vector3::Dot(axes[2] * obb.size.z, axis));

		// AABBとOBBの中心の距離
		float distance = std::abs(Vector3::Dot(toCenter, axis));

		// 分離軸テスト（衝突していない場合は離れている）
		if (distance > aProj + bProj)
		{
			// 非衝突
			return false;
		}
	}

	// 衝突している
	return true;
}

std::string CollisionManager::GetColliderTypeString(ColliderType type) const
{
	switch (type)
	{
	case ColliderType::AABB:
		return "AABB";
	case ColliderType::Sphere:
		return "Sphere";
	case ColliderType::OBB:
		return "OBB";
	}
	return "Unknown";
}

void CollisionManager::LogCollision(const std::string& phase, const ICollisionComponent* a, const ICollisionComponent* b)
{
#ifdef _DEBUG   // デバッグビルド時のみログを出力
	std::string tagA = a->GetOwner()->GetTag();
	std::string tagB = b->GetOwner()->GetTag();
	std::string typeAString = GetColliderTypeString(a->GetColliderType());
	std::string typeBString = GetColliderTypeString(b->GetColliderType());

	Logger::Log("| Collision " + phase + " " +
				(phase == "Exit" ? "<-" : (phase == "Enter" ? "->" : "=="))
				+ " | " + tagA + ": " + typeAString + ", " + tagB + ": " + typeBString + "\n");
#endif
}
