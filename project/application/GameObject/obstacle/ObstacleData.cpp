#include "ObstacleData.h"
#include "imgui/imgui.h"

ObstacleData::ObstacleData()
{
	REGISTER_MEMBER(obstacles); // 障害物のTransform情報を登録
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