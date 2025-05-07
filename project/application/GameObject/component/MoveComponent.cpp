#include "MoveComponent.h"

#include "math/MathUtils.h"

void MoveComponent::Update(GameObject* owner)
{
    Vector3 translation = { 0.0f, 0.0f, 0.0f };

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
    owner->SetPosition(currentTranslate + translation);
}
