#include "SplineData.h"
#include <fstream>

#include "base/Logger.h"
#include "imgui/imgui.h"

SplineData::SplineData()
{
	Register("controlPoints", &controlPoints);
}

void SplineData::Initialize(const std::string& name)
{
	LoadJson(name);
}
