#pragma once
#include <memory>

#include "application/GameObject/base/IGameObjectComponent.h"
#include "application/GameObject/weapon/Bullet.h"
#include "input/Input.h"
#include "math/MathUtils.h"
#include "math/Vector3.h"

class FireComponent : public IGameObjectComponent {
public:
    FireComponent();
    ~FireComponent();

    void Update(GameObject* owner) override;

private:
    void FireBullet(GameObject* owner);
    Object3dCommon* object3dCommon = nullptr;
    LightManager* lightManager = nullptr;

    float fireCooldown_;  // 発射のクールダウン時間
    float fireCooldownTimer_;  // 現在のクールダウンタイマー
    std::vector<std::shared_ptr<Bullet>> bullets_;  // 発射された弾のリスト
};
