#include "AccelerationComponent.h"

#include "base/GraphicsTypes.h"

AccelerationComponent::AccelerationComponent(const Vector3& accel)
    : acceleration_(accel)
{
}

void AccelerationComponent::Update(Particle& particle)
{
    particle.velocity += acceleration_;
}

nlohmann::json AccelerationComponent::SerializeToJson() const
{
	nlohmann::json json;
	json["type"] = GetComponentType();
	json["acceleration"] = {
		{"x", acceleration_.x},
		{"y", acceleration_.y},
		{"z", acceleration_.z}
	};
	return json;
}

void AccelerationComponent::DeserializeFromJson(const nlohmann::json& json)
{
	if (json.contains("acceleration"))
	{
		acceleration_= Vector3(
			json["acceleration"]["x"].get<float>(),
			json["acceleration"]["y"].get<float>(),
			json["acceleration"]["z"].get<float>()
		);
	}
}

void AccelerationComponent::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Text("Acceleration Component");
	ImGui::DragFloat3("Acceleration", &acceleration_.x, 0.01f);
#endif
}

