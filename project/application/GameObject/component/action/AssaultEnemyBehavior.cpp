#include "AssaultEnemyBehavior.h"
#include "AssaultRifleComponent.h"
#include "application/GameObject/base/GameObject.h"
#include "math/MathUtils.h"
#include "input/Input.h"
#include "line/LineManager.h"
#include "lighting/VectorColorCodes.h"

AssaultEnemyBehavior::AssaultEnemyBehavior(GameObject* target) : target_(target)
{
    // 初期パトロールポイントの設定
    patrolPoints_ = {
        Vector3(10.0f, 0.0f, 10.0f),
        Vector3(-10.0f, 0.0f, 10.0f),
        Vector3(-10.0f, 0.0f, -10.0f),
        Vector3(10.0f, 0.0f, -10.0f)
    };

    // 初期状態設定
    if (target)
    {
        currentState_ = State::Engage;
    }
    else
    {
        currentState_ = State::Patrol;
    }
}

void AssaultEnemyBehavior::Update(GameObject* owner)
{
    // クールダウン更新
    if (burstCooldown_ > 0)
    {
        burstCooldown_ -= 1.0f / 60.0f;
    }

    // 位置記録更新タイマー
    positionUpdateTimer_ += 1.0f / 60.0f;
    if (positionUpdateTimer_ >= 1.0f)
    {  // 1秒ごとに有効な位置を記録
        lastValidPosition_ = owner->GetPosition();
        positionUpdateTimer_ = 0.0f;
    }

    // ターゲットが設定されていなければ巡回モードに設定
    if (!target_ && currentState_ != State::Patrol)
    {
        currentState_ = State::Patrol;
    }

    // ターゲットとの距離チェック（テレポート防止）
    if (target_)
    {
        Vector3 targetPos = target_->GetPosition();
        Vector3 direction = targetPos - owner->GetPosition();
        float distance = direction.Length();

        // 最大距離を超えたら帰還モードに切り替え
        if (distance > maxDistance_ && currentState_ != State::Return)
        {
            currentState_ = State::Return;
            stateTimer_ = 0.0f;
        }
    }

    // ステートマシン
    switch (currentState_)
    {
    case State::Patrol:
        PatrolBehavior(owner);
        break;
    case State::Engage:
        EngageBehavior(owner);
        break;
    case State::Strafe:
        StrafeBehavior(owner);
        break;
    case State::Reposition:
        RepositionBehavior(owner);
        break;
    case State::Cover:
        CoverBehavior(owner);
        break;
    case State::Return:
        ReturnBehavior(owner);
        break;
    }

    // 状態タイマー更新
    stateTimer_ += 1.0f / 60.0f;
}

void AssaultEnemyBehavior::PatrolBehavior(GameObject* owner)
{
    // ターゲットが視界内にいれば戦闘モードへ
    if (target_ && IsPlayerVisible(owner))
    {
        currentState_ = State::Engage;
        stateTimer_ = 0.0f;
        return;
    }

    // パトロールポイントに向かって移動
    if (!patrolPoints_.empty())
    {
        Vector3 targetPoint = patrolPoints_[currentPatrolIndex_];
        Vector3 direction = targetPoint - owner->GetPosition();

        // 到着したら次のポイントへ
        if (direction.Length() < 1.0f)
        {
            if (patrolReverse_)
            {
                currentPatrolIndex_--;
                if (currentPatrolIndex_ < 0)
                {
                    currentPatrolIndex_ = 1;
                    patrolReverse_ = false;
                }
            }
            else
            {
                currentPatrolIndex_++;
                if (currentPatrolIndex_ >= patrolPoints_.size())
                {
                    currentPatrolIndex_ = patrolPoints_.size() - 2;
                    patrolReverse_ = true;
                }
            }
            return;
        }

        // 移動処理
        Vector3 normalizedDir = direction;  // ベクトルをコピー
        normalizedDir.Normalize();  // 正規化

        // 距離と速度に応じた移動（急激な位置変更を防止）
        float moveDistance = std::min(moveSpeed_ * 0.5f * (1.0f / 60.0f), direction.Length());
        owner->SetPosition(owner->GetPosition() + normalizedDir * moveDistance);

        // 移動方向に向かせる
        float angle = atan2(normalizedDir.x, normalizedDir.z);
        owner->SetRotation(Vector3(0, angle, 0));
    }
}

void AssaultEnemyBehavior::EngageBehavior(GameObject* owner)
{
    if (!target_)
    {
        currentState_ = State::Patrol;
        return;
    }

    Vector3 targetPos = target_->GetPosition();
    Vector3 direction = targetPos - owner->GetPosition();
    float distance = direction.Length();

    // プレイヤーとの距離に応じて行動を変える
    if (distance > attackRange_ * 1.5f)
    {
        // プレイヤーが遠い場合は近づく
        currentState_ = State::Reposition;
        stateTimer_ = 0.0f;
    }
    else if (distance < attackRange_ * 0.5f)
    {
        // プレイヤーが近すぎる場合は距離を取る
        Vector3 normalizedDir = direction;  // ベクトルをコピー
        normalizedDir.Normalize();  // 正規化
        Vector3 retreatDir = -normalizedDir;  // 反転して後退ベクトルを作成

        // 速度制限をかけて移動（テレポート防止）
        float moveDistance = std::min(moveSpeed_ * (1.0f / 60.0f), distance * 0.1f);
        owner->SetPosition(owner->GetPosition() + retreatDir * moveDistance);
    }
    else
    {
        // 最適距離ならバースト攻撃
        if (burstCooldown_ <= 0.0f)
        {
            FireWeapon(owner);
            burstCount_++;

            if (burstCount_ >= maxBurstCount_)
            {
                burstCount_ = 0;
                burstCooldown_ = 1.5f;

                // バースト後は横移動へ
                currentState_ = State::Strafe;
                stateTimer_ = 0.0f;
                strafeAngle_ = (rand() % 2 == 0) ? 0.0f : 3.14159f; // 左右ランダム
            }
        }
    }

    // プレイヤーの方向に向ける
    Vector3 normalizedDir = direction;
    normalizedDir.Normalize();
    float angle = atan2(normalizedDir.x, normalizedDir.z);
    owner->SetRotation(Vector3(0, angle, 0));

    // 一定時間経過で横移動に変更
    if (stateTimer_ > 3.0f)
    {
        currentState_ = State::Strafe;
        stateTimer_ = 0.0f;
        strafeAngle_ = (rand() % 2 == 0) ? 0.0f : 3.14159f; // 左右ランダム
    }
}

void AssaultEnemyBehavior::StrafeBehavior(GameObject* owner)
{
    if (!target_)
    {
        currentState_ = State::Patrol;
        return;
    }

    Vector3 targetPos = target_->GetPosition();
    Vector3 toTarget = targetPos - owner->GetPosition();
    float distance = toTarget.Length();

    // 距離が適正範囲外なら位置調整
    if (distance < attackRange_ * 0.5f || distance > attackRange_ * 1.5f)
    {
        currentState_ = State::Reposition;
        stateTimer_ = 0.0f;
        return;
    }

    // 横移動ベクトルの計算
    Vector3 forward = toTarget;
    forward.Normalize();
    Vector3 right = Vector3(forward.z, 0, -forward.x); // 直交ベクトル

    // サイン波で揺らす横移動
    strafeAngle_ += 0.05f;
    float strafeValue = sin(strafeAngle_);
    Vector3 strafeDir = right * strafeValue;

    // 横移動を適用（距離に応じて移動量を制限）
    float moveDistance = moveSpeed_ * 0.8f * (1.0f / 60.0f);
    owner->SetPosition(owner->GetPosition() + strafeDir * moveDistance);

    // プレイヤーの方向を向き続ける
    float angle = atan2(forward.x, forward.z);
    owner->SetRotation(Vector3(0, angle, 0));

    // ときどき攻撃
    if (burstCooldown_ <= 0.0f && stateTimer_ > 0.5f)
    {
        FireWeapon(owner);
        burstCount_++;

        if (burstCount_ >= 3)
        { // 短いバースト
            burstCount_ = 0;
            burstCooldown_ = 1.0f;
        }
    }

    // 横移動終了条件
    if (stateTimer_ > 2.5f)
    {
        currentState_ = State::Engage;
        stateTimer_ = 0.0f;
    }
}

void AssaultEnemyBehavior::RepositionBehavior(GameObject* owner)
{
    if (!target_)
    {
        currentState_ = State::Patrol;
        return;
    }

    Vector3 targetPos = target_->GetPosition();
    Vector3 direction = targetPos - owner->GetPosition();
    float distance = direction.Length();

    // 最適な射撃距離に調整
    float optimalDistance = attackRange_ * 0.8f;

    if (distance > optimalDistance)
    {
        // 接近（距離に応じて移動量を調整）
        Vector3 normalizedDir = direction;
        normalizedDir.Normalize();

        // 移動距離制限（テレポート防止）
        float moveDistance = std::min(moveSpeed_ * 1.2f * (1.0f / 60.0f), distance * 0.05f);
        owner->SetPosition(owner->GetPosition() + normalizedDir * moveDistance);
    }
    else
    {
        // 最適距離に達したら戦闘モードに
        currentState_ = State::Engage;
        stateTimer_ = 0.0f;
    }

    // プレイヤーの方向に向ける
    Vector3 normalizedDir = direction;
    normalizedDir.Normalize();
    float angle = atan2(normalizedDir.x, normalizedDir.z);
    owner->SetRotation(Vector3(0, angle, 0));
}

void AssaultEnemyBehavior::CoverBehavior(GameObject* owner)
{
    // 掩蓋行動（実装は環境依存）
    // 一定時間後に戦闘状態に戻る
    if (stateTimer_ > 3.0f)
    {
        currentState_ = State::Engage;
        stateTimer_ = 0.0f;
    }
}

// 新規追加：プレイヤー付近に戻る動作（テレポート防止用）
void AssaultEnemyBehavior::ReturnBehavior(GameObject* owner)
{
    if (!target_)
    {
        currentState_ = State::Patrol;
        return;
    }

    Vector3 targetPos = target_->GetPosition();
    Vector3 direction = targetPos - owner->GetPosition();
    float distance = direction.Length();

    // 徐々に近づく（テレポートではなくスムーズな移動）
    Vector3 normalizedDir = direction;
    normalizedDir.Normalize();

    // 速度と距離に応じた移動量調整
    float moveDistance = std::min(moveSpeed_ * 1.5f * (1.0f / 60.0f), distance * 0.05f);

    // 移動
    owner->SetPosition(owner->GetPosition() + normalizedDir * moveDistance);

    // プレイヤーの方向を向く
    float angle = atan2(normalizedDir.x, normalizedDir.z);
    owner->SetRotation(Vector3(0, angle, 0));

    // プレイヤーとの距離が適正になったら戦闘モードに戻る
    if (distance < attackRange_ * 1.2f)
    {
        currentState_ = State::Engage;
        stateTimer_ = 0.0f;
    }

    // 長時間プレイヤーに近づけない場合は最後の有効位置に移動
    if (stateTimer_ > 5.0f)
    {
        // 前回記録した有効な位置へ移動
        owner->SetPosition(lastValidPosition_);
        currentState_ = State::Engage;
        stateTimer_ = 0.0f;
    }
}

void AssaultEnemyBehavior::FireWeapon(GameObject* owner)
{
    // 実際の発射処理はここでAssaultRifleComponentを呼び出す
    // 例：owner->GetComponent<AssaultRifleComponent>("weapon")->Fire();

    // デバッグ表示用
#ifdef _DEBUG
    // 発射方向に線を描画
    Vector3 forward = GetForwardVector(owner);
    Vector3 start = owner->GetPosition();
    Vector3 end = start + forward * 10.0f;
	LineManager::GetInstance()->DrawLine(start, end, VectorColorCodes::Red);
#endif
}

bool AssaultEnemyBehavior::IsPlayerVisible(GameObject* owner)
{
    if (!target_) return false;

    Vector3 targetPos = target_->GetPosition();
    Vector3 direction = targetPos - owner->GetPosition();
    float distance = direction.Length();

    // 距離チェック
    if (distance > detectionRange_)
    {
        return false;
    }

    // 視線チェック（環境によって実装が異なる）
    // ここでは単純に距離だけでチェック
    return true;
}

Vector3 AssaultEnemyBehavior::GetForwardVector(GameObject* owner)
{
    // オブジェクトの回転から前方ベクトルを計算
    Vector3 rotation = owner->GetRotation();
    float angleY = rotation.y;

    // Y軸回転から前方向ベクトルを計算
    Vector3 forward;
    forward.x = sin(angleY);
    forward.y = 0.0f;
    forward.z = cos(angleY);

    return forward;
}