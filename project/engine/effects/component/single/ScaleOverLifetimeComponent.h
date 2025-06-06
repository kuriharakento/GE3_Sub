#pragma once
#include "effects/component/interface/IParticleBehaviorComponent.h"

class ScaleOverLifetimeComponent : public IParticleBehaviorComponent
{
public:
    ScaleOverLifetimeComponent(float start, float end);
    void Update(Particle& particle) override;

	// パーティクルエディタ用
	std::string GetComponentType() const override { return "ScaleOverLifetimeComponent"; }
	nlohmann::json SerializeToJson() const override;
	void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;

private:
    float startScale_;
    float endScale_;
};
