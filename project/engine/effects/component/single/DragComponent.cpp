#include "DragComponent.h"

#include "base/GraphicsTypes.h"

DragComponent::DragComponent(float drag)
    : dragFactor_(drag)
{
}

void DragComponent::Update(Particle& particle)
{
    particle.velocity *= dragFactor_;
}

nlohmann::json DragComponent::SerializeToJson() const
{
	nlohmann::json json;
	json["type"] = GetComponentType();
	json["dragFactor"] = dragFactor_;
	return json;
}

void DragComponent::DeserializeFromJson(const nlohmann::json& json)
{
	if (json.contains("dragFactor"))
	{
		dragFactor_ = json["dragFactor"].get<float>();
	}
}

void DragComponent::DrawImGui()
{
#ifdef _DEBUG
	ImGui::Text("Drag Component");
	ImGui::DragFloat("Drag Factor", &dragFactor_, 0.01f);
#endif
}
