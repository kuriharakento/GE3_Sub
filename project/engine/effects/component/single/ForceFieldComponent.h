#pragma once
#include "effects/component/interface/IParticleBehaviorComponent.h"
#include "base/GraphicsTypes.h"

// 特定の中心点からの引力または斥力をパーティクルに与えるコンポーネント
class ForceFieldComponent : public IParticleBehaviorComponent
{
public:
    enum class ForceType
    {
        Attract, // 引力
        Repel    // 斥力
    };

    explicit ForceFieldComponent(const Vector3& center, float strength, float maxDistance, ForceType type = ForceType::Attract);
    void Update(Particle& particle) override;

	// パーティクルエディタ用
	std::string GetComponentType() const override { return "ForceFieldComponent"; }
    nlohmann::json SerializeToJson() const override;
	void DeserializeFromJson(const nlohmann::json& json) override;
	void DrawImGui() override;

private:
    Vector3 forceCenter;
    float strength;    // 力の強さ
    float maxDistance; // この距離外のパーティクルには影響しない
    ForceType type;
};