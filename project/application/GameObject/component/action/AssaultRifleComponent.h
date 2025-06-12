#pragma once
#include "application/effect/AssaultRifleHitEffect.h"
#include "application/GameObject/component/base/IActionComponent.h"
#include "application/GameObject/weapon/Bullet.h"

class AssaultRifleComponent : public IActionComponent
{
public:
    AssaultRifleComponent(Object3dCommon* object3dCommon, LightManager* lightManager);
    ~AssaultRifleComponent();

    void Update(GameObject* owner) override;
    void Draw(CameraManager* camera) override;

private:
    void FireBullet(GameObject* owner);
    void FireBullet(GameObject* owner, const Vector3& targetPosition);
    void StartReload();
    void Reload(float deltaTime);

    Object3dCommon* object3dCommon_ = nullptr;
    LightManager* lightManager_ = nullptr;

    float fireCooldown_;
    float fireCooldownTimer_;
    std::vector<std::unique_ptr<Bullet>> bullets_;

    // 弾数・リロード
    int maxAmmo_ = 30;
    int currentAmmo_ = 30;
    bool isReloading_ = false;
    float reloadTime_ = 2.0f;
    float reloadTimer_ = 0.0f;

    //ヒットエフェクト
	std::unique_ptr<AssaultRifleHitEffect> hitEffect_;
};
