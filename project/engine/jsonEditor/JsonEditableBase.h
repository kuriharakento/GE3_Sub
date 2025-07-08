#pragma once
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "IJsonEditable.h"
#include "imgui/imgui.h"
#include <fstream>
#include "math/Vector3.h"
#include <type_traits>
#include <vector>
#include "base/GraphicsTypes.h"
#include "JsonSerialization.h"

class JsonEditableBase : public IJsonEditable
{
public:
	JsonEditableBase() = default;
	bool LoadJson(const std::string& path) override;
	bool SaveJson(const std::string& path) const override;
	void DrawImGui() override;
	virtual void DrawOptions();

protected:
	template<typename T>
	// NOTE: ここでは変数名と同じキー名で登録する必要がある
	void Register(const std::string& name, T* value);

private:
	std::unordered_map<std::string, std::function<nlohmann::json()>> getters_;
	std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> setters_;
	std::unordered_map<std::string, std::function<void()>> drawers_;

	const std::string dirPath = "Resources/json/";
	std::string fileName;
};

// type-trait to detect std::vector<...>
template<typename> struct is_std_vector : std::false_type {};
template<typename U, typename A> struct is_std_vector<std::vector<U, A>> : std::true_type {};

template<typename T>
void JsonEditableBase::Register(const std::string& name, T* value)
{
	if (getters_.count(name)) return;

	// シンプルに全体型に対して to_json/from_json を丸投げ
	getters_[name] = [value]() {
		return nlohmann::json(*value);
		};
	setters_[name] = [value](const nlohmann::json& j) {
		j.get_to(*value);
		};

	// --- ImGui 描画関数登録 ---
	drawers_[name] = [value, name]() {
		ImGui::PushID(name.c_str());
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			// float
			if constexpr (std::is_same_v<T, float>)
			{
				ImGui::DragFloat("##val", value, 0.1f);
			}
			// int
			else if constexpr (std::is_same_v<T, int>)
			{
				ImGui::DragInt("##val", value);
			}
			// bool
			else if constexpr (std::is_same_v<T, bool>)
			{
				ImGui::Checkbox("##val", value);
			}
			// Vector3
			else if constexpr (std::is_same_v<T, Vector3>)
			{
				// &v.x を渡すことで float[3] にキャスト可
				ImGui::DragFloat3("##val", &value->x, 0.1f);
			}
			// Transform - 追加！
			else if constexpr (std::is_same_v<T, Transform>)
			{
				ImGui::Text("Transform");
				ImGui::DragFloat3("Translate", &value->translate.x, 0.1f);
				ImGui::DragFloat3("Rotate", &value->rotate.x, 0.01f);
				ImGui::DragFloat3("Scale", &value->scale.x, 0.1f);
				
			}
			// std::vector<Transform> - 追加！
			else if constexpr (std::is_same_v<T, std::vector<Transform>>)
			{
				for (size_t i = 0; i < value->size(); ++i)
				{
					std::string headerLabel = "Transform[" + std::to_string(i) + "]";
					ImGui::PushID(static_cast<int>(i));

					if (ImGui::TreeNode(headerLabel.c_str()))
					{
						ImGui::DragFloat3("Translate", &(*value)[i].translate.x, 0.1f);
						ImGui::DragFloat3("Rotate", &(*value)[i].rotate.x, 0.01f);
						ImGui::DragFloat3("Scale", &(*value)[i].scale.x, 0.1f);
						ImGui::TreePop();
					}

					ImGui::PopID();
				}

				// 配列の操作ボタン
				ImGui::Separator();
				if (ImGui::Button("Add Transform"))
				{
					value->push_back(Transform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} });
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove Last") && !value->empty())
				{
					value->pop_back();
				}
			}
			// std::vector<Vector3> - 追加！
			else if constexpr (std::is_same_v<T, std::vector<Vector3>>)
			{
				for (size_t i = 0; i < value->size(); ++i)
				{
					std::string label = "Element[" + std::to_string(i) + "]";
					ImGui::DragFloat3(label.c_str(), &(*value)[i].x, 0.1f);
				}
			}
			// std::string
			else if constexpr (std::is_same_v<T, std::string>)
			{
				char buf[256];
				// 安全版 strncpy_s
				strncpy_s(buf, sizeof(buf), value->c_str(), _TRUNCATE);
				if (ImGui::InputText("##val", buf, sizeof(buf)))
				{
					*value = buf;
				}
			}
			// std::vector<std::string> - 追加！
			else if constexpr (std::is_same_v<T, std::vector<std::string>>)
			{
				for (size_t i = 0; i < value->size(); ++i)
				{
					std::string label = "Element[" + std::to_string(i) + "]";
					char buf[256];
					strncpy_s(buf, sizeof(buf), (*value)[i].c_str(), _TRUNCATE);
					if (ImGui::InputText(label.c_str(), buf, sizeof(buf)))
					{
						(*value)[i] = buf;
					}
				}
			}
			// その他は raw JSON マルチラインで編集
			else
			{
				std::string s = nlohmann::json(*value).dump(2);
				char* buf = (char*)alloca(s.size() + 1);
				memcpy(buf, s.c_str(), s.size() + 1);
				if (ImGui::InputTextMultiline(
					"##val", buf, s.size() + 1,
					ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 8),
					ImGuiInputTextFlags_AllowTabInput |
					ImGuiInputTextFlags_AlwaysOverwrite))
				{
					try
					{
						auto jj = nlohmann::json::parse(buf);
						*value = jj.get<T>();
					}
					catch (...) {}
				}
			}
		}
		ImGui::PopID();
		};
}