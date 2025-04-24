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

void SplineData::DrawImGui()
{
	//ポイインタを使用して一意のIDを生成
	ImGui::PushID(this);

	ImGui::SameLine();
	if (ImGui::Button("Add ControlPoints"))
	{
		controlPoints.push_back(Vector3(0.0f, 0.0f, 0.0f));
	}

	JsonEditableBase::DrawImGui();

	//IDをポップ
	ImGui::PopID();
}
