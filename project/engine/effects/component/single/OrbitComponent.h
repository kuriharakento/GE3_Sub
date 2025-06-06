#pragma once
#include <cmath>

#include "effects/component/interface/IParticleBehaviorComponent.h"

class OrbitComponent : public IParticleBehaviorComponent
{
public:
    OrbitComponent(const Vector3& c, float radius_, float speed);
	OrbitComponent(const Vector3* target, float radius_, float speed);
    void Update(Particle& particle) override;

	// パーティクルエディタ用
	std::string GetComponentType() const override { return "OrbitComponent"; }
	nlohmann::json SerializeToJson() const override;
	void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;

private:
	const Vector3* target_ = nullptr; // 追従対象の位置
    Vector3 center_;
    float angularSpeed_;
    float radius_;
};
