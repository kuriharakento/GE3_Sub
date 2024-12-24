#pragma once

#include "Mech.h"
#include "application/Collision/ICollidable.h"

class Enemy : public Mech, public ICollidable
{
public:
    // コンストラクタ
    Enemy()
    {
        status_.health = 50.0f;          // 初期体力
        status_.attackPower = 5.0f;      // 初期攻撃力
        status_.speed = 0.02f;           // 初期速度
        hitBox_.min = Vector3(-0.5f, -0.5f, -0.5f); // 衝突判定の最小座標
        hitBox_.max = Vector3(0.5f, 0.5f, 0.5f);    // 衝突判定の最大座標
        type_ = ObjectType::Enemy;       // オブジェクトタイプ
    }

    // 初期化処理
    void Initialize(const std::string& filePath, Object3dCommon* objectCommon);

    // 更新処理
    void Update(CameraManager* camera = nullptr);

    // 描画処理
    void Draw();

    // 衝突時の処理
    void OnCollision(ICollidable* other) override;

public: // アクセッサ

    ObjectType GetType() const override { return type_; }

    float GetHealth() const override { return status_.health; }

    float GetAttackPower() const override { return status_.attackPower; }

    float GetSpeed() const override { return status_.speed; }

    const AABB& GetBoundingBox() const override { return hitBox_; }

private:
    // オブジェクトのTransform情報を更新
    void UpdateObjTransform();

    // 衝突判定用のAABB
    AABB hitBox_;

    // 敵の種類などを識別するための情報（必要に応じて追加）
    std::string enemyType_;
};

inline void Enemy::UpdateObjTransform()
{
    object3d_->SetScale(transform_.scale);
    object3d_->SetRotate(transform_.rotate);
    object3d_->SetTranslate(transform_.translate);
    object3d_->Update();
}
