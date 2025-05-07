#pragma once
#include <application/GameObject/base/GameObject.h>
#include <application/GameObject/base/IGameObjectComponent.h>
#include "input/Input.h"
#include "math/Vector3.h"

class MoveComponent : public IGameObjectComponent {
public:
    MoveComponent(float speed) : speed_(speed) {}

    void Update(GameObject* owner) override;

private:
    float speed_; // 移動速度
};
