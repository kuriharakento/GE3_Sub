#pragma once
#include "externals/nlohmann/json.hpp"
#include "math/Vector3.h"

using json = nlohmann::json;

class SplineData
{
public:
	bool LoadJson(const std::string& filePath);
	bool SaveJson(const std::string& filePath);
	const std::vector<Vector3>& GetControlPoints() const { return controlPoints; }
private:
	std::vector<Vector3> controlPoints; // 制御点の座標
};

