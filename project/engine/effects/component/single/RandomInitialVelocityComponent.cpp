#include "RandomInitialVelocityComponent.h"

RandomInitialVelocityComponent::RandomInitialVelocityComponent(const Vector3& minV, const Vector3& maxV)
    : minVelocity_(minV), maxVelocity_(maxV)
{
}

void RandomInitialVelocityComponent::Update(Particle& particle)
{
    if (!initialized_)
    {
        particle.velocity.x = RandomFloat(minVelocity_.x, maxVelocity_.x);
        particle.velocity.y = RandomFloat(minVelocity_.y, maxVelocity_.y);
        particle.velocity.z = RandomFloat(minVelocity_.z, maxVelocity_.z);
        initialized_ = true;
    }
}

nlohmann::json RandomInitialVelocityComponent::SerializeToJson() const
{
	nlohmann::json json;
	json["type"] = GetComponentType();
	json["minVelocity"] = {
		{"x", minVelocity_.x},
		{"y", minVelocity_.y},
		{"z", minVelocity_.z}
	};
	json["maxVelocity"] = {
		{"x", maxVelocity_.x},
		{"y", maxVelocity_.y},
		{"z", maxVelocity_.z}
	};
	return json;
}

void RandomInitialVelocityComponent::DeserializeFromJson(const nlohmann::json& json)
{
	if (json.contains("minVelocity"))
	{
		minVelocity_ = Vector3(
			json["minVelocity"]["x"].get<float>(),
			json["minVelocity"]["y"].get<float>(),
			json["minVelocity"]["z"].get<float>()
		);
	}
	if (json.contains("maxVelocity"))
	{
		maxVelocity_ = Vector3(
			json["maxVelocity"]["x"].get<float>(),
			json["maxVelocity"]["y"].get<float>(),
			json["maxVelocity"]["z"].get<float>()
		);
	}
}

void RandomInitialVelocityComponent::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Text("Random Initial Velocity Component");
	ImGui::DragFloat3("Min Velocity", &minVelocity_.x, 0.01f);
	ImGui::DragFloat3("Max Velocity", &maxVelocity_.x, 0.01f);
#endif
}

float RandomInitialVelocityComponent::RandomFloat(float min, float max)
{
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return min + r * (max - min);
}
