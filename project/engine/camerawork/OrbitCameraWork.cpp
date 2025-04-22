#include "OrbitCameraWork.h"
#include "math/MathUtils.h"

void OrbitCameraWork::Initialize(Camera* camera)
{
	camera_ = camera;
	time_ = 0.0f;
	isActive_ = false;
}

void OrbitCameraWork::Update()
{
    if (!isActive_) return;

    Vector3 targetPosition = targetPtr_ ? *targetPtr_ : targetValue_;

    // カメラ位置を円軌道で計算
    Vector3 cameraPosition = targetPosition + Vector3(cos(time_) * radius_, 0.0f, sin(time_) * radius_);
    camera_->SetTranslate(cameraPosition);

    // ターゲット方向のベクトルを取得
    Vector3 toTarget = targetPosition - cameraPosition;

    // Y軸回りの角度（Yaw）を求める
    float yaw = std::atan2(toTarget.x, toTarget.z);

    // ピッチ（上下の傾き）を求めたい場合は、Y成分も考慮
    float horizontalDist = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
    float pitch = std::atan2(toTarget.y, horizontalDist);

    // カメラの回転を設定（X:ピッチ, Y:ヨー, Zは0）
    camera_->SetRotate(Vector3(pitch, yaw, 0.0f));

    time_ += speed_ * 0.016f;
}

void OrbitCameraWork::Start(Vector3 target, float radius, float speed)
{
	targetValue_ = target;
	radius_ = radius;
	speed_ = speed;
	isActive_ = true;
}

void OrbitCameraWork::Start(Vector3* target, float radius, float speed)
{
	targetPtr_ = target;
	radius_ = radius;
	speed_ = speed;
	isActive_ = true;
}
