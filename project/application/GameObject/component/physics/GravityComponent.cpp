#include "GravityComponent.h"
#include "application/GameObject/base/GameObject.h"

GravityComponent::GravityComponent(float gravity)
    : gravity_(gravity), verticalVelocity_(0.0f)
{
}

void GravityComponent::Update(GameObject* owner)
{
    // フレーム時間（deltaTime）は 1.0f/60.0f と仮定
    float deltaTime = 1.0f / 60.0f;
    
    // 重力による垂直速度の更新
    verticalVelocity_ -= gravity_ * deltaTime;
    
    // 現在のポジションを取得
    Vector3 currentPosition = owner->GetPosition();
    
    // Y軸方向に垂直速度を加算
    currentPosition.y += verticalVelocity_ * deltaTime;
    
    // 新しいポジションを設定
    owner->SetPosition(currentPosition);
}

void GravityComponent::Draw(CameraManager* camera)
{
    // 空実装
}