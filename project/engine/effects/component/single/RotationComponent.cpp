#include "RotationComponent.h"

RotationComponent::RotationComponent(const Vector3& rotSpeed)
    : rotationSpeed_(rotSpeed)
{
}

void RotationComponent::Update(Particle& particle)
{
    particle.transform.rotate += rotationSpeed_;
}

nlohmann::json RotationComponent::SerializeToJson() const
{
	nlohmann::json json;
	json["type"] = GetComponentType();
	json["rotationSpeed"] = {
		{"x", rotationSpeed_.x},
		{"y", rotationSpeed_.y},
		{"z", rotationSpeed_.z}
	};
	return json;
}

void RotationComponent::DeserializeFromJson(const nlohmann::json& json)
{
	if (json.contains("rotationSpeed"))
	{
		rotationSpeed_ = Vector3(
			json["rotationSpeed"]["x"].get<float>(),
			json["rotationSpeed"]["y"].get<float>(),
			json["rotationSpeed"]["z"].get<float>()
		);
	}
}

void RotationComponent::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Text("Rotation Component");
	ImGui::DragFloat3("Rotation Speed", &rotationSpeed_.x, 0.01f);
#endif
}

