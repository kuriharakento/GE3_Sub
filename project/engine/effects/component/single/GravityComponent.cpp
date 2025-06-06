#include "GravityComponent.h"

#include "base/GraphicsTypes.h"

GravityComponent::GravityComponent(const Vector3& g)
    : gravity(g)
{
}

void GravityComponent::Update(Particle& particle)
{
    particle.velocity += gravity;
}

nlohmann::json GravityComponent::SerializeToJson() const
{
	nlohmann::json json;
	json["type"] = GetComponentType();
	json["gravity"] = {
		{"x", gravity.x},
		{"y", gravity.y},
		{"z", gravity.z}
	};
	return json;
}

void GravityComponent::DeserializeFromJson(const nlohmann::json& json)
{
	if (json.contains("gravity"))
	{
		gravity = Vector3(
			json["gravity"]["x"].get<float>(),
			json["gravity"]["y"].get<float>(),
			json["gravity"]["z"].get<float>()
		);
	}
}

void GravityComponent::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Text("Gravity Component");
	ImGui::DragFloat3("Gravity", &gravity.x, 0.01f);
#endif
}