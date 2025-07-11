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
    JsonEditableBase::DrawImGui();
}