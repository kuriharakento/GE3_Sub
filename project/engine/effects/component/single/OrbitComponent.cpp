#include "OrbitComponent.h"

OrbitComponent::OrbitComponent(const Vector3& c, float radius_, float speed)
    : center_(c), radius_(radius_), angularSpeed_(speed)
{
}

OrbitComponent::OrbitComponent(const Vector3* target, float radius_, float speed) : target_(target), radius_(radius_), angularSpeed_(speed)
{
	if (target_)
	{
		center_ = *target_;
	}
	else
	{
        center_ = Vector3();
	}
}

void OrbitComponent::Update(Particle& particle)
{
	if (target_)
	{
		center_ = *target_;
	}

    float angle = angularSpeed_;

    Vector3 offset = particle.transform.translate - center_;

    float cosA = std::cos(angle);
    float sinA = std::sin(angle);

    float x = offset.x * cosA - offset.z * sinA;
    float z = offset.x * sinA + offset.z * cosA;

    offset.x = x;
    offset.z = z;

    particle.transform.translate = center_ + offset;
}

nlohmann::json OrbitComponent::SerializeToJson() const
{
	nlohmann::json json;
	json["type"] = GetComponentType();
	json["center"] = {
		{"x", center_.x},
		{"y", center_.y},
		{"z", center_.z}
	};
	json["radius"] = radius_;
	json["angularSpeed"] = angularSpeed_;
	return json;
}

void OrbitComponent::DeserializeFromJson(const nlohmann::json& json)
{
	if (json.contains("center"))
	{
		center_ = Vector3(
			json["center"]["x"].get<float>(),
			json["center"]["y"].get<float>(),
			json["center"]["z"].get<float>()
		);
	}
	if (json.contains("radius"))
	{
		radius_ = json["radius"].get<float>();
	}
	if (json.contains("angularSpeed"))
	{
		angularSpeed_ = json["angularSpeed"].get<float>();
	}
}

void OrbitComponent::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Text("Orbit Component");
	ImGui::DragFloat3("Center", &center_.x, 0.01f);
	ImGui::DragFloat("Radius", &radius_, 0.01f);
	ImGui::DragFloat("Angular Speed", &angularSpeed_, 0.01f);
#endif
}
