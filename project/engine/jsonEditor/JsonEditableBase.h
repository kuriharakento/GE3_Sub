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
	bool LoadJson(const std::string& path) override;
	bool SaveJson(const std::string& path) const override;
	void DrawImGui() override;
    virtual void DrawOptions();
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
    // すでに登録されている場合は何もしない
    if (getters_.find(name) != getters_.end()) {
        return;
    }

    getters_[name] = [value]() { return nlohmann::json(*value); };
    setters_[name] = [value](const nlohmann::json& j) { *value = j.get<T>(); };
    drawers_[name] = [value, name]() {
        if (ImGui::CollapsingHeader(name.c_str()))  // コラプスヘッダーでまとめる
        {
            // 型に応じたUI描画
            if constexpr (std::is_same_v<T, float>)
                ImGui::DragFloat(name.c_str(), value, 0.1f);
            else if constexpr (std::is_same_v<T, int>)
                ImGui::DragInt(name.c_str(), value);
            else if constexpr (std::is_same_v<T, bool>)
                ImGui::Checkbox(name.c_str(), value);
            else if constexpr (std::is_same_v<T, Vector3>)
                ImGui::DragFloat3(name.c_str(), &value->x, 0.1f);
            else if constexpr (std::is_same_v<T, std::vector<Vector3>>)
            {
                for (size_t i = 0; i < value->size(); ++i)
                {
                    std::string label = name + "[" + std::to_string(i) + "]";
                    ImGui::DragFloat3(label.c_str(), &(*value)[i].x, 0.1f);
                }
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                // std::stringを編集するための一時的なバッファ
                char buffer[256];
                strncpy_s(buffer, value->c_str(), sizeof(buffer));
                buffer[sizeof(buffer) - 1] = '\0'; // バッファの終端を保証

                if (ImGui::InputText(name.c_str(), buffer, sizeof(buffer)))
                {
                    *value = buffer; // 編集結果をstd::stringに反映
                }
            }
            else if constexpr (std::is_same_v<T, std::vector<std::string>>)
            {
                for (size_t i = 0; i < value->size(); ++i)
                {
                    std::string label = name + "[" + std::to_string(i) + "]";
                    char buffer[256];
                    strncpy_s(buffer, (*value)[i].c_str(), sizeof(buffer));
                    buffer[sizeof(buffer) - 1] = '\0'; // バッファの終端を保証

                    if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer)))
                    {
                        (*value)[i] = buffer; // 編集結果をstd::stringに反映
                    }
                }
            }
        }
    };
}

