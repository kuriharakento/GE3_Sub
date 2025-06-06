#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "ImGui/imgui.h"

class IParticleComponent
{
public:
    virtual ~IParticleComponent() = default;

    // コンポーネントタイプの識別子を返す
	virtual std::string GetComponentType() const = 0;

	// コンポーネントパラメーターをJson形式で取得する
	virtual nlohmann::json SerializeToJson() const = 0;

	// Jsonからパラメーターを設定する
	virtual void DeserializeFromJson(const nlohmann::json& json) = 0;

	// ImGuiでのパラメーター編集を行う
	virtual void DrawImGui() = 0;
};