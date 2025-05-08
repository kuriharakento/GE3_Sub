#pragma once
#include <application/GameObject/base/GameObject.h>
#include <application/GameObject/base/IGameObjectComponent.h>
#include "input/Input.h"
#include "math/Vector3.h"

class MoveComponent : public IGameObjectComponent {
public:
    MoveComponent(float speed) : speed_(speed) {}

    void Update(GameObject* owner) override;
    void Draw(CameraManager* camera) override {};

private:
    float speed_; // 移動速度

    // ダッシュ
    bool isDashing_ = false;
    float dashDuration_ = 0.2f;       // ダッシュの持続時間
    float dashCooldown_ = 1.0f;       // ダッシュのクールダウン
    float dashTimer_ = 0.0f;          // ダッシュの残り時間
    float dashCooldownTimer_ = 0.0f;  // ダッシュのクールダウンタイマー
    float dashMultiplier_ = 2.5f;     // ダッシュ時の速度倍率

    // 回避
    bool isDodging_ = false;
    float dodgeDuration_ = 0.2f;      // 回避の持続時間（秒）を短く設定
    float dodgeCooldown_ = 0.8f;      // 回避のクールダウン時間（秒）
    float dodgeTimer_ = 0.0f;         // 回避の残り時間
    float dodgeCooldownTimer_ = 0.0f; // 回避のクールダウンタイマー
    float dodgeSpeed_ = 15.0f;        // 回避中の速度を大きく設定
    Vector3 dodgeDirection_ = { 0.0f, 0.0f, 0.0f }; // 回避方向
};
