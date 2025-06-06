#pragma once
#include "base/GraphicsTypes.h"
#include "effects/component/interface/IParticleBehaviorComponent.h"

class RotationComponent : public IParticleBehaviorComponent
{
public:
    explicit RotationComponent(const Vector3& rotSpeed);
    void Update(Particle& particle) override;

	// パーティクルエディタ用
	std::string GetComponentType() const override { return "RotationComponent"; }
	nlohmann::json SerializeToJson() const override;
	void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;

private:
    Vector3 rotationSpeed_;
};
