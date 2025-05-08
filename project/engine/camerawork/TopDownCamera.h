#pragma once

#include "CameraWorkBase.h"
#include "math/Vector3.h"

class Camera;

class TopDownCamera : public CameraWorkBase
{
public:
    void Initialize(Camera* camera)override;
    void Update() override;
    void Start(float height, const Vector3* target);

    void SetTarget(const Vector3* target);
    void SetHeight(float height);
    void SetActive(bool active);

private:
    Camera* camera_ = nullptr;
    const Vector3* target_ = nullptr; // 追従するターゲット
    float height_ = 10.0f;            // カメラの高さ
    bool isActive_ = false;
};
