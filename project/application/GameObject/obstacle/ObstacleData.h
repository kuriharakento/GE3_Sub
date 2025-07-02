#pragma once
#include "externals/nlohmann/json.hpp"
#include "jsonEditor/JsonEditableBase.h"
#include "math/Vector3.h"

using json = nlohmann::json;

class ObstacleData : public JsonEditableBase
{
public:
    ObstacleData();
    void Initialize(const std::string& name);
    void AddObstacle(const Vector3& position, const Vector3& rotation, const Vector3& scale);
    const std::vector<Vector3>& GetPositions() const { return positions; }
    const std::vector<Vector3>& GetRotations() const { return rotations; }
    const std::vector<Vector3>& GetScales() const { return scales; }
    void DrawImGui() override;

	uint32_t GetObstacleCount() const { return static_cast<uint32_t>(positions.size() + rotations.size() + scales.size()) / 3; }

private:
    std::vector<Vector3> positions;  // 障害物の位置
    std::vector<Vector3> rotations;  // 障害物の回転
    std::vector<Vector3> scales;     // 障害物のスケール
};