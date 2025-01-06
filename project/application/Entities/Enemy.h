#pragma once

#include "Mech.h"
#include "application/Collision/ICollidable.h"
#include "application/Weapon/Missile.h"

//行動パターン
enum class State
{
    Idle,     // 待機
	Chase,    // 追跡
	Approach, // 接近
};

class Player;
class Enemy : public Mech, public ICollidable
{
public:
    // コンストラクタ
    Enemy()
    {
		state_ = State::Idle;            // 初期ステート
		status_.isAlive = true;         // 生存フラグ
        status_.health = 50.0f;          // 初期体力
        status_.attackPower = 5.0f;      // 初期攻撃力
        status_.speed = 0.02f;           // 初期速度
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

	bool IsAlive() const { return status_.isAlive; }

public: // アクセッサ

    ObjectType GetType() const override { return type_; }

    float GetHealth() const override { return status_.health; }

    float GetAttackPower() const override { return status_.attackPower; }

    float GetSpeed() const override { return status_.speed; }

    const AABB& GetBoundingBox() const override { return hitBox_; }

	void SetPlayer(Player* player) { player_ = player; }

	void SetPosition(const Vector3& position) override;
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }

    Vector3 GetPosition() const override { return transform_.translate; }
	Vector3 GetScale() const { return transform_.scale; }
	Vector3 GetRotate() const { return transform_.rotate; }

	//ミサイルのリストを取得
	std::vector<std::unique_ptr<Missile>>& GetMissiles() { return missiles_; }
	Missile* GetMissile(int index) { return missiles_[index].get(); }

private:
    // オブジェクトのTransform情報を更新
    void UpdateObjTransform(CameraManager* camera);

    // 待機状態の更新
    void UpdateIdle();

    // 巡回状態の更新
    void UpdatePatrol();

    // 攻撃状態の更新
    void UpdateAttack();

    // 退避状態の更新
    void UpdateApproach();

    // プレイヤーとの距離チェック
    bool IsPlayerInRange(float range);

    // ミサイルを発射
    void FireMissile();

private: // メンバ変数
    //プレイヤーのポインタ
	Player* player_ = nullptr;

	// 3Dオブジェクト共通情報
	Object3dCommon* objectCommon_ = nullptr;

    //ミサイルリスト
    std::vector<std::unique_ptr<Missile>> missiles_;

    // 行動管理
    State state_ = State::Idle;         // 現在のステート
    float fireInterval_ = 2.0f;  // ミサイル発射間隔
    float fireTimer_ = 0.0f;     // ミサイル発射タイマー
    float patrolRange_ = 20.0f;   // 巡回範囲
    // ランダムな巡回方向
    Vector3 patrolDirection_ = Vector3(1.0f, 0.0f, 0.0f); // 初期は右方向に設定
    // 巡回タイマー
    float patrolTimer_ = 0.0f;
    float retreatRange_ = 10.0f;  // 退避範囲
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
    // 無効なミサイルを削除
    missiles_.erase(
        std::remove_if(missiles_.begin(), missiles_.end(), [](const std::unique_ptr<Missile>& missile) {
            return !missile->IsAlive();
            }),
        missiles_.end());
}
