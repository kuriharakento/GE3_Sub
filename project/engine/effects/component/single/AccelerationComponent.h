#pragma once
#include "effects/component/interface/IParticleBehaviorComponent.h"

class AccelerationComponent : public IParticleBehaviorComponent
{
public:
    explicit AccelerationComponent(const Vector3& accel);
    void Update(Particle& particle) override;

    //　パーティクルエディタ用
    std::string GetComponentType() const override { return "GravityComponent"; }
    nlohmann::json SerializeToJson() const override;
    void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;

private:
    Vector3 acceleration_;
};
