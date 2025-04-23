#pragma once
#include "externals/nlohmann/json.hpp"
#include "jsonEditor/JsonEditableBase.h"
#include "math/Vector3.h"

using json = nlohmann::json;

class SplineData : public JsonEditableBase
{
public:
	SplineData();
	void Initialize(const std::string& name);
	const std::vector<Vector3>& GetControlPoints() const { return controlPoints; }
private:
	std::vector<Vector3> controlPoints; // 制御点の座標
};

