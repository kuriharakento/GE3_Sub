#include "MoveComponent.h"
#include "math/MathUtils.h"

void MoveComponent::Update(GameObject* owner)
{
    Vector3 translation = { 0.0f, 0.0f, 0.0f };

    // 現在の時間を取得（フレーム単位で管理）
    float deltaTime = 1.0f / 60.0f;

    // ダッシュのクールダウンを更新
    if (dashCooldownTimer_ > 0.0f) {
        dashCooldownTimer_ -= deltaTime;
    }

    // 回避のクールダウンを更新
    if (dodgeCooldownTimer_ > 0.0f) {
        dodgeCooldownTimer_ -= deltaTime;
    }

    // 回避中の処理
    if (isDodging_) {
        dodgeTimer_ -= deltaTime;
        if (dodgeTimer_ <= 0.0f) {
            isDodging_ = false; // 回避終了
        }
        else {
            // 回避中は一定の速度で移動
            owner->SetPosition(owner->GetPosition() + dodgeDirection_ * dodgeSpeed_ * deltaTime);
            return; // 回避中は他の処理を無視
        }
    }

    // WASDキーの入力を取得
    if (Input::GetInstance()->PushKey(DIK_W)) {
        translation.z += speed_;
    }
    if (Input::GetInstance()->PushKey(DIK_S)) {
        translation.z -= speed_;
    }
    if (Input::GetInstance()->PushKey(DIK_A)) {
        translation.x -= speed_;
    }
    if (Input::GetInstance()->PushKey(DIK_D)) {
        translation.x += speed_;
    }

    // ダッシュの処理
    if (Input::GetInstance()->PushKey(DIK_LSHIFT) && dashCooldownTimer_ <= 0.0f) {
        isDashing_ = true;
    }
    else {
        isDashing_ = false;
    }

    if (isDashing_) {
        translation *= dashMultiplier_; // ダッシュ中は速度を増加
    }

    // 回避の処理
    if (Input::GetInstance()->TriggerKey(DIK_SPACE) && dodgeCooldownTimer_ <= 0.0f) {
        isDodging_ = true;
        dodgeTimer_ = dodgeDuration_;
        dodgeCooldownTimer_ = dodgeCooldown_;

        // 移動方向を回避方向に設定
        if (translation.LengthSquared() > 0.0f) {
            dodgeDirection_ = translation.Normalize();
        }
        else {
            dodgeDirection_ = { 0.0f, 0.0f, 1.0f }; // デフォルトで前方に回避
        }
        return; // 回避中は他の処理を無視
    }

    // 移動が発生している場合のみ回転を更新
    if (translation.LengthSquared() > 0.0f) {
        // 移動ベクトルを正規化
        Vector3 direction = translation.Normalize();

        // Y軸回りの目標回転角度を計算（atan2を使用）
        float targetRotationY = atan2f(direction.x, direction.z);

        // 現在の回転を取得
        Vector3 currentRotation = owner->GetRotation();

        // 角度を正規化（-π ～ π の範囲に収める）
        float normalizedCurrentY = MathUtils::NormalizeAngleRad(currentRotation.y);
        float normalizedTargetY = MathUtils::NormalizeAngleRad(targetRotationY);

        // イージングを使用して滑らかに回転
        float easedRotationY = MathUtils::Lerp(
            normalizedCurrentY,
            normalizedTargetY,
            0.1f // 補間速度（0.0f～1.0f）
        );

        // 回転を更新
        owner->SetRotation({ currentRotation.x, easedRotationY, currentRotation.z });
    }

    // 現在の位置に加算
    auto currentTranslate = owner->GetPosition();
    owner->SetPosition(currentTranslate + translation * deltaTime);
}
