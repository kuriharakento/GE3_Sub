#pragma once
#include "effects/component/interface/IParticleBehaviorComponent.h"

class DragComponent : public IParticleBehaviorComponent
{
public:
    explicit DragComponent(float drag);
    void Update(Particle& particle) override;

	// パーティクルエディタ用
	std::string GetComponentType() const override { return "DragComponent"; }
    nlohmann::json SerializeToJson() const override;
	void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;

private:
    float dragFactor_;
};
