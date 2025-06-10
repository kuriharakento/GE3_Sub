#pragma once
#include "effects/ParticleGroup.h"
#include "effects/component/interface/IParticleGroupComponent.h"
#include "math/Vector3.h"

class UVTranslateComponent : public IParticleGroupComponent
{
public:
    explicit UVTranslateComponent(const Vector3& translate);

    void Update(ParticleGroup& group) override;

	std::string GetComponentType() const override { return "UVTranslateComponent"; }
    nlohmann::json SerializeToJson() const override;;
	void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;

private:
    Vector3 translate_;
};
