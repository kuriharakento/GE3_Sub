#pragma once
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "IJsonEditable.h"
#include "imgui/imgui.h"
#include "math/Vector3.h"
#include <type_traits>
#include <vector>
#include "base/GraphicsTypes.h"
#include "JsonSerialization.h"
#include "JsonEditorImGuiUtils.h"

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
	// NOTE: 必ず変数は登録すること!! しないとエラーが出る。
	// NOTE: ここでは変数名と同じキー名で登録する必要がある。
	void Register(const std::string& name, T* value);

private:
	std::unordered_map<std::string, std::function<nlohmann::json()>> getters_;
	std::unordered_map<std::string, std::function<void(const nlohmann::json&)>> setters_;
	std::unordered_map<std::string, std::function<void()>> drawers_;

	const std::string dirPath = "Resources/json/";
	std::string fileName;
};

// メンバ変数登録の自動化マクロ
#define REGISTER_MEMBER(var) Register(#var, &var)

// 型チェックのためのヘルパー
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
			// 型ごとの描画関数に委譲
			if constexpr (std::is_same_v<T, float>)
				DrawImGuiForFloat("##val", value);
			else if constexpr (std::is_same_v<T, int>)
				DrawImGuiForInt("##val", value);
			else if constexpr (std::is_same_v<T, bool>)
				DrawImGuiForBool("##val", value);
			else if constexpr (std::is_same_v<T, Vector3>)
				DrawImGuiForVector3("##val", value);
			else if constexpr (std::is_same_v<T, Transform>)
				DrawImGuiForTransform("##val", value);
			else if constexpr (std::is_same_v<T, std::vector<Transform>>)
				DrawImGuiForTransformVector("##val", value);
			else if constexpr (std::is_same_v<T, std::vector<Vector3>>)
				DrawImGuiForVector3Vector("##val", value);
			else if constexpr (std::is_same_v<T, std::string>)
				DrawImGuiForString("##val", value);
			else if constexpr (std::is_same_v<T, std::vector<std::string>>)
				DrawImGuiForStringVector("##val", value);
			else
				DrawImGuiForRawJson<T>("##val", value);
		}
		ImGui::PopID();
		};
}