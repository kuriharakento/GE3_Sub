#pragma once
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "IJsonEditable.h"
#include "imgui/imgui.h"
#include <fstream>
#include "math/Vector3.h"
#include "JsonUtils.h"
#include <type_traits>
#include <vector>
#include "base/GraphicsTypes.h"
#include "TransformSerialization.h"
#include "Vector3Serialization.h"

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

	// --- JSON シリアライズ / デシリアライズ ---
	if constexpr (is_std_vector<T>::value)
	{
		// std::vector<U> 用
		using U = typename T::value_type;
		getters_[name] = [value]() {
			nlohmann::json arr = nlohmann::json::array();
			for (auto& e : *value)
			{
				nlohmann::json je;
				// U に対して to_json/free 関数が必要
				nlohmann::adl_serializer<U>::to_json(je, e);
				arr.push_back(je);
			}
			return arr;
			};
		setters_[name] = [value](auto const& arr) {
			auto& vec = *value;
			vec.clear();
			for (auto& je : arr)
			{
				U e{};
				nlohmann::adl_serializer<U>::from_json(je, e);
				vec.push_back(e);
			}
			};
	}
	else
	{
		// それ以外は汎用 ADL シリアライザ
		getters_[name] = [value]() {
			nlohmann::json j;
			nlohmann::adl_serializer<T>::to_json(j, *value);
			return j;
			};
		setters_[name] = [value](auto const& j) {
			nlohmann::adl_serializer<T>::from_json(j, *value);
			};
	}

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
			// その他（Transform も含む）は raw JSON マルチラインで編集
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