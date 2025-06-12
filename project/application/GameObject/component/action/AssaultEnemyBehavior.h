#pragma once
#include <vector>

#include "application/GameObject/component/base/IGameObjectComponent.h"
#include "math/Vector3.h"

class GameObject;

class AssaultEnemyBehavior : public IGameObjectComponent
{
public:
    AssaultEnemyBehavior(GameObject* target);
    void Update(GameObject* owner) override;

    // 設定メソッド
    void SetTarget(GameObject* target) { target_ = target; }
    void SetMoveSpeed(float speed) { moveSpeed_ = speed; }
    void SetAttackRange(float range) { attackRange_ = range; }
    void SetDetectionRange(float range) { detectionRange_ = range; }

private:
    enum class State
    {
        Patrol,     // 巡回
        Engage,     // 戦闘態勢
        Strafe,     // 横移動
        Reposition, // 位置調整
        Cover,      // 掩蓋
        Return      // プレイヤーの元へ戻る（テレポート防止用）
    };

    void PatrolBehavior(GameObject* owner);   // 巡回行動
    void EngageBehavior(GameObject* owner);   // 攻撃態勢行動
    void StrafeBehavior(GameObject* owner);   // 横移動行動
    void RepositionBehavior(GameObject* owner); // 移動行動
    void CoverBehavior(GameObject* owner);    // 掩蓋行動
    void ReturnBehavior(GameObject* owner);   // 帰還行動（新規追加）

    bool IsPlayerVisible(GameObject* owner);  // プレイヤーが視界に入っているか
    void FireWeapon(GameObject* owner);       // 武器発射
    Vector3 GetForwardVector(GameObject* owner); // 前方向ベクトル取得

    State currentState_ = State::Patrol;
    GameObject* target_ = nullptr;            // 追跡対象（プレイヤー）

    // パラメーター
    float stateTimer_ = 0.0f;                 // 状態タイマー
    float moveSpeed_ = 5.0f;                  // 移動速度
    float attackRange_ = 15.0f;               // 攻撃範囲
    float detectionRange_ = 25.0f;            // 検出範囲
    float maxDistance_ = 50.0f;               // プレイヤーとの最大許容距離
    float strafeRadius_ = 8.0f;               // 横移動半径
    float strafeAngle_ = 0.0f;                // 横移動角度
    float burstCooldown_ = 0.0f;              // バースト攻撃クールダウン
    int burstCount_ = 0;                      // バースト弾数カウント
    int maxBurstCount_ = 5;                   // 最大バースト弾数

    // 巡回ポイント
    std::vector<Vector3> patrolPoints_;
    int currentPatrolIndex_ = 0;
    bool patrolReverse_ = false;              // 巡回方向反転

    // 移動関連
    Vector3 lastValidPosition_;               // 最後の有効な位置
    float positionUpdateTimer_ = 0.0f;        // 位置更新タイマー
};