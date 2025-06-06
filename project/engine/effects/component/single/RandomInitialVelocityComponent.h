#pragma once
#include <cstdlib>

#include "effects/component/interface/IParticleBehaviorComponent.h"

class RandomInitialVelocityComponent : public IParticleBehaviorComponent
{
public:
    RandomInitialVelocityComponent(const Vector3& minV, const Vector3& maxV);
    void Update(Particle& particle) override;

	// パーティクルエディタ用
	std::string GetComponentType() const override { return "RandomInitialVelocityComponent"; }
	nlohmann::json SerializeToJson() const override;
	void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;

private:
    float RandomFloat(float min, float max);
    Vector3 minVelocity_;
    Vector3 maxVelocity_;
    bool initialized_ = false;
};
