#pragma once

#include "math/Vector3.h"
#include <memory>

#include "application/GameObject/base/GameObject.h"

class Bullet : public GameObject {
public:
    Bullet();
    virtual ~Bullet();

    void Initialize(Object3dCommon* object3dCommon, LightManager* lightManager, const Vector3& position, const Vector3& direction, float speed, float lifetime);
    void Update(float deltaTime);
    void Draw(CameraManager* camera);

    bool IsAlive() const { return isAlive_; }

private:
    Vector3 direction_;  // 移動方向
    float speed_;        // 速度
    float lifetime_;     // 寿命
    float elapsedTime_;  // 経過時間
    bool isAlive_;       // 生死状態
};
