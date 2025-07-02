#include "ObstacleData.h"
#include "imgui/imgui.h"

ObstacleData::ObstacleData()
{
    Register("positions", &positions);
    Register("rotations", &rotations);
    Register("scales", &scales);
}

void ObstacleData::Initialize(const std::string& name)
{
    LoadJson(name);
}

void ObstacleData::AddObstacle(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
    positions.push_back(position);
    rotations.push_back(rotation);
    scales.push_back(scale);
}

void ObstacleData::DrawImGui()
{
    // オプション
    ImGui::SameLine();
    if (ImGui::Button("Add Obstacle"))
    {
        AddObstacle(
            Vector3(0.0f, 0.0f, 0.0f),
            Vector3(0.0f, 0.0f, 0.0f),
            Vector3(1.0f, 1.0f, 1.0f)
        );
    }

    // 情報表示
    JsonEditableBase::DrawImGui();
}