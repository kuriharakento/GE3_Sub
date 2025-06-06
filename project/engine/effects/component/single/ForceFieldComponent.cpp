#include "ForceFieldComponent.h"

ForceFieldComponent::ForceFieldComponent(const Vector3& center, float strength, float maxDistance, ForceType type)
    : forceCenter(center)
    , strength(strength)
    , maxDistance(maxDistance)
    , type(type)
{
}

void ForceFieldComponent::Update(Particle& particle)
{
    Vector3 direction = forceCenter - particle.transform.translate;
    float distanceSq = direction.LengthSquared(); // 距離の二乗

    if (distanceSq < maxDistance * maxDistance && distanceSq > 0.0001f) // 0除算回避
    {
        float distance = std::sqrt(distanceSq);
        // 距離が離れるほど力が弱まる（逆二乗の法則など）
        // ここでは単純に距離に反比例させる
        float forceMagnitude = strength / distance;

        // 力の方向を正規化
        direction.Normalize();

        if (type == ForceType::Repel)
        {
            forceMagnitude *= -1.0f; // 斥力の場合は逆方向
        }

        // 力を速度に加算
        particle.velocity += direction * forceMagnitude;
    }
}

nlohmann::json ForceFieldComponent::SerializeToJson() const
{
	nlohmann::json json;
	json["type"] = GetComponentType();
    json["forceCenter"] = {
        {"x", forceCenter.x},
        {"y", forceCenter.y},
        {"z", forceCenter.z}
    };
	json["strength"] = strength;
	json["maxDistance"] = maxDistance;
	json["forceType"] = (type == ForceType::Attract) ? "Attract" : "Repel";
	return json;
}

void ForceFieldComponent::DeserializeFromJson(const nlohmann::json& json)
{
	if (json.contains("forceCenter"))
	{
		forceCenter = Vector3(
			json["forceCenter"]["x"].get<float>(),
			json["forceCenter"]["y"].get<float>(),
			json["forceCenter"]["z"].get<float>()
		);
	}
	if (json.contains("strength"))
	{
		strength = json["strength"].get<float>();
	}
	if (json.contains("maxDistance"))
	{
		maxDistance = json["maxDistance"].get<float>();
	}
	if (json.contains("forceType"))
	{
		std::string typeStr = json["forceType"].get<std::string>();
		type = (typeStr == "Attract") ? ForceType::Attract : ForceType::Repel;
	}
}

void ForceFieldComponent::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Text("Force Field Component");
	ImGui::DragFloat3("Force Center", &forceCenter.x, 0.01f);
	ImGui::DragFloat("Strength", &strength, 0.01f);
	ImGui::DragFloat("Max Distance", &maxDistance, 0.01f);
	const char* forceTypeStr = (type == ForceType::Attract) ? "Attract" : "Repel";
	if (ImGui::BeginCombo("Force Type", forceTypeStr))
	{
		if (ImGui::Selectable("Attract")) type = ForceType::Attract;
		if (ImGui::Selectable("Repel")) type = ForceType::Repel;
		ImGui::EndCombo();
	}
#endif
}
