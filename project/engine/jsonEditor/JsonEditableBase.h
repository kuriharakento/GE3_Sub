#pragma once
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "IJsonEditable.h"
#include "imgui/imgui.h"
#include <fstream>
#include "math/Vector3.h"
#include "JsonUtils.h"

class JsonEditableBase : public IJsonEditable
{
public:
	void LoadJson(const std::string& path) override;
	void SaveJson(const std::string& path) const override;
	void DrawImGui() override;

protected:
	template<typename T>
	//NOTE:ここでは絶対に変数名と同じ名前を登録しないといけない
	void Register(const std::string& name, T* value);

private:
	std::unordered_map<std::string, std::function<nlohmann::json()>> getters_;
	std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> setters_;
	std::unordered_map<std::string, std::function<void()>> drawers_;
    const std::string dirPath = "Resources/json/";
    std::string fileName;
};

template<typename T>
void JsonEditableBase::Register(const std::string& name, T* value)
{
    getters_[name] = [value]() { return nlohmann::json(*value); };
    setters_[name] = [value](const nlohmann::json& j) { *value = j.get<T>(); };
    drawers_[name] = [value, name]() {
        if constexpr (std::is_same_v<T, float>)
            ImGui::DragFloat(name.c_str(), value, 0.1f);
        else if constexpr (std::is_same_v<T, int>)
            ImGui::DragInt(name.c_str(), value);
        else if constexpr (std::is_same_v<T, Vector3>)
            ImGui::DragFloat3(name.c_str(), &value->x, 0.1f);
        else if constexpr (std::is_same_v<T, std::vector<Vector3>>)
            for (size_t i = 0; i < value->size(); ++i) {
                std::string label = name + "[" + std::to_string(i) + "]";
                ImGui::DragFloat3(label.c_str(), &(*value)[i].x, 0.1f);
            }
        };
}

