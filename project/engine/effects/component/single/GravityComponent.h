#pragma once
#include "effects/component/interface/IParticleBehaviorComponent.h"

class GravityComponent : public IParticleBehaviorComponent
{
public:
    explicit GravityComponent(const Vector3& g);
    void Update(Particle& particle) override;

	// パーティクルエディタ用
	std::string GetComponentType() const override { return "GravityComponent"; }
	nlohmann::json SerializeToJson() const override;
	void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;

private:
	Vector3 gravity; // 重力ベクトル
};
