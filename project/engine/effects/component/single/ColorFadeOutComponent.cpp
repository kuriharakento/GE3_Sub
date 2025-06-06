#include "ColorFadeOutComponent.h"

#include "base/GraphicsTypes.h"

void ColorFadeOutComponent::Update(Particle& particle)
{
    float lifeRatio = particle.currentTime / particle.lifeTime;
    if (lifeRatio > 1.0f) lifeRatio = 1.0f;
    particle.color.w = 1.0f - lifeRatio;
}

nlohmann::json ColorFadeOutComponent::SerializeToJson() const
{
	nlohmann::json json;
	json["type"] = GetComponentType();
	return json;
}

void ColorFadeOutComponent::DeserializeFromJson(const nlohmann::json& json)
{
	// 特にパラメータはないので空実装
}

void ColorFadeOutComponent::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Text("Color Fade Out Component");
#endif
}
