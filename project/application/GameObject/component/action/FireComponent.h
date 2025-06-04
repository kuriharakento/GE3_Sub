#pragma once
#include <memory>

#include "application/GameObject/component/base/IActionComponent.h"
#include "application/GameObject/weapon/Bullet.h"
#include "input/Input.h"
#include "math/MathUtils.h"
#include "math/Vector3.h"

class FireComponent : public IActionComponent
{
public:
	FireComponent(Object3dCommon* object3dCommon, LightManager* lightManager);
	~FireComponent();

	void Update(GameObject* owner) override;
	void Draw(CameraManager* camera) override;

private:
	void FireBullet(GameObject* owner);
	Object3dCommon* object3dCommon_ = nullptr;
	LightManager* lightManager_ = nullptr;

	float fireCooldown_;  // 発射のクールダウン時間
	float fireCooldownTimer_;  // 現在のクールダウンタイマー
	std::vector<std::unique_ptr<Bullet>> bullets_;  // 発射された弾のリスト
};
