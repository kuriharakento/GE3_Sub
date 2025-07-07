#pragma once
#include "application/GameObject/component/base/IActionComponent.h"
#include "math/Vector3.h"

class GameObject;

class GravityComponent : public IActionComponent
{
public:
    explicit GravityComponent(float gravity = 9.8f);
    void Update(GameObject* owner) override;
    void Draw(CameraManager* camera) override;

private:
    float gravity_;           // 重力加速度
    float verticalVelocity_;  // 現在の垂直速度
};