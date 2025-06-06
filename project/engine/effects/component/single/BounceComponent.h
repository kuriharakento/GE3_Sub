#pragma once
#include "effects/component/interface/IParticleBehaviorComponent.h"

class BounceComponent : public IParticleBehaviorComponent
{
public:
	explicit BounceComponent(float groundHeight, float restitution, float minVelocity);
	void Update(Particle& particle) override;

	// パーティクルエディタ用
	std::string GetComponentType() const override { return "BounceComponent"; }
	nlohmann::json SerializeToJson() const override;
	void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;

private:
	float groundHeight_; // 地面の高さ
	float restitution_; // 反発係数
	float minVelocity_; // 最小速度
};

