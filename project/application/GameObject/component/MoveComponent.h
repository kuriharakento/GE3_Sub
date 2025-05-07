#pragma once
#include <application/GameObject/base/GameObject.h>
#include <application/GameObject/base/IGameObjectComponent.h>
#include "input/Input.h"
#include "math/Vector3.h"

class MoveComponent : public IGameObjectComponent {
public:
    MoveComponent(float speed) : speed_(speed) {}

    void Update(GameObject* owner) override {
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

        // 現在の位置に加算
        auto currentTranslate = owner->GetPosition();
		owner->SetPosition(currentTranslate + translation);
    }

private:
    float speed_; // 移動速度
};
