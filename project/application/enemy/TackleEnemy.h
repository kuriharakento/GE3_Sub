#pragma once
#include "BaseEnemy.h"
class TackleEnemy : public BaseEnemy
{
public:
	void Initialize(Object3dCommon* object3dCommon, const std::string& filePath) override;
	void Update(CameraManager* camera) override;
	void Draw() override;
	void SetLightManager(LightManager* lightManager) { object3d_->SetLightManager(lightManager); }
	void StartTackle(const Vector3& targetPosition);
	void UpdateTackle();
	Vector3 GetTargetPosition() const { return target_; }
	void SetTargetPosition(const Vector3& target) { target_ = target; }
	ModelData GetModelData() const { return object3d_->GetModelData(); }
	Vector3 GetPosition() const { return transform_.translate; }
private:
	bool isTackling_;
	float tackleSpeed_;
	float tackleAcceleration_;
	float tackleDuration_;
	float tackleTimer_;
	Vector3 tackleDirection_;
	Vector3 target_;
};

