#include "ScaleOverLifetimeComponent.h"

ScaleOverLifetimeComponent::ScaleOverLifetimeComponent(float start, float end)
    : startScale_(start), endScale_(end)
{
}

void ScaleOverLifetimeComponent::Update(Particle& particle)
{
    float lifeRatio = particle.currentTime / particle.lifeTime;
    if (lifeRatio > 1.0f) lifeRatio = 1.0f;
    float scale = startScale_ + (endScale_ - startScale_) * lifeRatio;
    particle.transform.scale = Vector3(scale, scale, scale);
}

nlohmann::json ScaleOverLifetimeComponent::SerializeToJson() const
{
	nlohmann::json json;
	json["type"] = GetComponentType();
	json["startScale"] = startScale_;
	json["endScale"] = endScale_;
	return json;
}

void ScaleOverLifetimeComponent::DeserializeFromJson(const nlohmann::json& json)
{
	if (json.contains("startScale"))
	{
		startScale_ = json["startScale"].get<float>();
	}
	if (json.contains("endScale"))
	{
		endScale_ = json["endScale"].get<float>();
	}
}

void ScaleOverLifetimeComponent::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Text("Scale Over Lifetime Component");
	ImGui::DragFloat("Start Scale", &startScale_, 0.01f);
	ImGui::DragFloat("End Scale", &endScale_, 0.01f);
#endif
}
