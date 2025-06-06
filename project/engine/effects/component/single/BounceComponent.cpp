#include "BounceComponent.h"

BounceComponent::BounceComponent(float groundHeight, float restitution, float minVelocity) : groundHeight_(groundHeight), restitution_(restitution), minVelocity_(minVelocity)
{
}

void BounceComponent::Update(Particle& particle)
{
	// 次のフレームでのY座標を予測
	float nextY = particle.transform.translate.y + particle.velocity.y * (1.0f / 60.0f);

	// 地面に到達
	if (particle.transform.translate.y >= groundHeight_ && nextY < groundHeight_)
	{
		particle.transform.translate.y = groundHeight_ + particle.transform.scale.y * 0.5f;
		particle.velocity.y = -particle.velocity.y * restitution_;
	}
}

nlohmann::json BounceComponent::SerializeToJson() const
{
	nlohmann::json json;
	json["type"] = GetComponentType();
	json["groundHeight"] = groundHeight_;
	json["restitution"] = restitution_;
	json["minVelocity"] = minVelocity_;
	return json;
}

void BounceComponent::DeserializeFromJson(const nlohmann::json& json)
{
	if (json.contains("groundHeight"))
	{
		groundHeight_ = json["groundHeight"].get<float>();
	}
	if (json.contains("restitution"))
	{
		restitution_ = json["restitution"].get<float>();
	}
	if (json.contains("minVelocity"))
	{
		minVelocity_ = json["minVelocity"].get<float>();
	}
}

void BounceComponent::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Text("Bounce Component");
	ImGui::DragFloat("Ground Height", &groundHeight_, 0.01f);
	ImGui::DragFloat("Restitution", &restitution_, 0.01f);
	ImGui::DragFloat("Min Velocity", &minVelocity_, 0.01f);
#endif
}

