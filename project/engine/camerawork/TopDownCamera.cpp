#include "TopDownCamera.h"

#include <numbers>
#include <assimp/MathFunctions.h>

#include "base/Camera.h"
#include "math/MathUtils.h"

void TopDownCamera::Initialize(Camera* camera)
{
    camera_ = camera;
}

void TopDownCamera::Update()
{
    if (!camera_ || !target_ || !isActive_) return;

    // ターゲットの位置を基準にカメラの位置を計算
    Vector3 targetPos = *target_;
    Vector3 targetCameraPos = targetPos + Vector3(0.0f, height_, 0.0f);

	//イージングを使用してカメラの位置を滑らかに移動
	Vector3 currentPosition = MathUtils::Lerp(camera_->GetTranslate(), targetCameraPos, 0.1f);

    // カメラの位置を設定
    camera_->SetTranslate(currentPosition);

	//カメラの向きを真下に向ける
	camera_->SetRotate(Vector3(std::numbers::pi_v<float> / 2, 0.0f, 0.0f));
}

void TopDownCamera::Start(float height, const Vector3* target)
{
	height_ = height;
	target_ = target;
	isActive_ = true;
}

void TopDownCamera::SetTarget(const Vector3* target)
{
    target_ = target;
}

void TopDownCamera::SetHeight(float height)
{
    height_ = height;
}

void TopDownCamera::SetActive(bool active)
{
    isActive_ = active;
}
