#include "ObstacleData.h"
#include "imgui/imgui.h"

ObstacleData::ObstacleData()
{
	Register("objects", &obstacles);
}

void ObstacleData::Initialize(const std::string& name)
{
    LoadJson(name);
}

void ObstacleData::AddObstacle(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
    
}

void ObstacleData::DrawImGui()
{
    // 情報表示
    //JsonEditableBase::DrawImGui();

	// ImGuiでの編集UIを描画
	ImGui::SeparatorText("Obstacle Settings");

	ImGui::Text("Obstacle Count: %zu", obstacles.size());
	if(ImGui::CollapsingHeader("Obstacles"))
	{
		for (size_t i = 0; i < obstacles.size(); ++i)
		{
			ImGui::PushID(static_cast<int>(i));
			ImGui::Text("Obstacle %zu", i + 1);
			ImGui::Text("Type: %s", obstacles[i].type.c_str());
			ImGui::Text("Name: %s", obstacles[i].name.c_str());
			DrawImGuiForTransform("Transform", &obstacles[i].transform);
			ImGui::PopID();
			if (i < obstacles.size() - 1)
			{
				ImGui::Separator();
			}
		}
	}
}