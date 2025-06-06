#pragma once
#include "effects/component/interface/IParticleBehaviorComponent.h"

class ColorFadeOutComponent : public IParticleBehaviorComponent
{
public:
    void Update(Particle& particle) override;

	// パーティクルエディタ用
	std::string GetComponentType() const override { return "ColorFadeOutComponent"; }
	nlohmann::json SerializeToJson() const override;
	void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;
};
