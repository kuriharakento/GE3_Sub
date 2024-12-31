#pragma once

#include "Mech.h"
#include "application/Collision/ICollidable.h"
#include "application/Weapon/Missile.h"

//行動パターン
enum class EnemyState
{
	Idle,       //待機
	Chase,      //追跡
	Attack,     //攻撃
	Escape,     //逃げる
};

class Player;
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

	void SetPlayer(Player* player) { player_ = player; }

	void SetPosition(const Vector3& position) { transform_.translate = position; }
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }

	Vector3 GetPosition() const { return transform_.translate; }
	Vector3 GetScale() const { return transform_.scale; }
	Vector3 GetRotate() const { return transform_.rotate; }

private:
    // オブジェクトのTransform情報を更新
    void UpdateObjTransform(CameraManager* camera);

	void UpdateMissile();

private: // メンバ変数
    //プレイヤーのポインタ
	Player* player_ = nullptr;

	// 3Dオブジェクト共通情報
	Object3dCommon* objectCommon_ = nullptr;

    //ミサイルリスト
    std::vector<std::unique_ptr<Missile>> missiles_;

    //ミサイル発射間隔
	float fireInterval_ = 2.0f;

    //次の発射までの時間
    float fireTimer_ = 0.0f;

};

inline void Enemy::UpdateObjTransform(CameraManager* camera)
{
    object3d_->SetScale(transform_.scale);
    object3d_->SetRotate(transform_.rotate);
    object3d_->SetTranslate(transform_.translate);
    object3d_->Update(camera);
    // ミサイルの更新
    for (auto& missile : missiles_)
    {
        missile->Update(camera);
    }
}
