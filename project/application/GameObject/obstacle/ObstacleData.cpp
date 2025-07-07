#include "ObstacleData.h"
#include "imgui/imgui.h"

ObstacleData::ObstacleData()
{
    Register("obstacles", &obstacles);
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
    // オプション
    //ImGui::SameLine();

    // 情報表示
    JsonEditableBase::DrawImGui();
}