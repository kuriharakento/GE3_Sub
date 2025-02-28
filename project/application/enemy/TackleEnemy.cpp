#include "TackleEnemy.h"
#include "math/Easing.h"
#include <chrono>

void TackleEnemy::Initialize(Object3dCommon* object3dCommon, const std::string& filePath)
{
    //オブジェクトの初期化
    object3d_ = std::make_unique<Object3d>();
    object3d_->Initialize(object3dCommon);
    object3d_->SetModel(filePath);
    //トランスフォームの初期化
    transform_ = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };

    // ランダムエンジンの初期化（シードを現在時刻から取得）
    randomEngine_.seed(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));

    // 次のタックルまでの待機時間を設定
    std::uniform_real_distribution<float> waitTimeDist(1.0f, 3.0f); // 1秒〜5秒の間でランダム
    nextTackleWaitTime_ = waitTimeDist(randomEngine_);
    tackleWaitTimer_ = 0.0f;
}

void TackleEnemy::Update(CameraManager* camera)
{
    // タックルの更新
    UpdateTackle();

    // タックル中でない場合、待機タイマーを更新
    if (!isTackling_)
    {
        tackleWaitTimer_ += 1.0f / 60.0f;
        if (tackleWaitTimer_ >= nextTackleWaitTime_)
        {
            // タックルを開始
            StartTackle();

            // 待機タイマーをリセット
            tackleWaitTimer_ = 0.0f;
        }
    }

    //オブジェクトの更新
    object3d_->SetTranslate(transform_.translate);
    object3d_->SetRotate(transform_.rotate);
    object3d_->SetScale(transform_.scale);
    object3d_->Update(camera);
}

void TackleEnemy::Draw()
{
    object3d_->Draw();
}

void TackleEnemy::StartTackle()
{
    if (!isTackling_) {
        isTackling_ = true;

        // ターゲット方向を計算
        tackleDirection_ = target_ - transform_.translate;
        // Y軸方向の移動はしない
        tackleDirection_.y = 0.0f;
        // 正規化
        tackleDirection_ = Vector3::Normalize(tackleDirection_);

        // 初期速度を設定
        tackleVelocity_ = tackleDirection_ * tackleSpeed_;

        // 次のタックルまでの待機時間を設定
        std::uniform_real_distribution<float> waitTimeDist(1.0f, 5.0f); // 1秒〜5秒の間でランダム
        nextTackleWaitTime_ = waitTimeDist(randomEngine_);
    }
}

void TackleEnemy::UpdateTackle()
{
    if (isTackling_) {
        // フレーム間の時間差（秒）
        float deltaTime = 1.0f / 60.0f;

        // 摩擦による減速を適用
        Vector3 friction = -tackleVelocity_ * tackleFriction_ * deltaTime;
        tackleVelocity_ += friction;

        // 速度が非常に小さくなったら停止する
        if (Vector3::Length(tackleVelocity_) < 0.01f) {
            tackleVelocity_ = { 0.0f, 0.0f, 0.0f };
            isTackling_ = false;
            return;
        }

        // 位置を更新
        transform_.translate += tackleVelocity_ * deltaTime;
    }
}

