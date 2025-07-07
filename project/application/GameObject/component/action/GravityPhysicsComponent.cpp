#include "GravityPhysicsComponent.h"
#include "application/GameObject/base/GameObject.h"

GravityPhysicsComponent::GravityPhysicsComponent(float gravity)
    : gravity_(gravity)
    , verticalVelocity_(0.0f)
{
}

void GravityPhysicsComponent::Update(GameObject* owner)
{
    // 60FPS 想定
    const float deltaTime = 1.0f / 60.0f;

    // 重力を垂直速度に適用（下方向は負）
    verticalVelocity_ -= gravity_ * deltaTime;

    // 位置を更新
    auto pos = owner->GetPosition();
    pos.y += verticalVelocity_ * deltaTime;

    // 地面（Y=0）より下に行かせない
    if (pos.y < 0.0f + owner->GetScale().y)
    {
        pos.y = owner->GetScale().y;
        verticalVelocity_ = 0.0f; // 着地で速度リセット
    }

    owner->SetPosition(pos);
}