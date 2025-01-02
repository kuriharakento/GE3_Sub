#pragma once
#include "AABB.h"
#include "ObjectType.h"

//衝突判定が必要なオブジェクトに継承させるインターフェース
class ICollidable {
public:
    virtual ~ICollidable() = default;

    // AABBの座標情報を取得
    virtual const AABB& GetBoundingBox() const = 0;

    //位置の取得
	virtual Vector3 GetPosition() const = 0;

    // 衝突時のコールバック関
    virtual void OnCollision(ICollidable* other) = 0;

    // オブジェクトの種類
    virtual ObjectType GetType() const = 0;

    // 攻撃力を取得（ない場合は0を返す）
    virtual float GetAttackPower() const { return 0.0f; }

	//速度を取得（ない場合は0を返す）
	virtual float GetSpeed() const { return 0.0f; }

	// 体力を取得（ない場合は0を返す）
	virtual float GetHealth() const { return 0.0f; }

protected:
	// オブジェクトの種類
	ObjectType type_ = ObjectType::None;
	//当たり判定用のAABB
	AABB hitBox_ = {};
};
