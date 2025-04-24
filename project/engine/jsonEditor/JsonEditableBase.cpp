#include "JsonEditableBase.h"

#include "base/Logger.h"

bool JsonEditableBase::LoadJson(const std::string& path)
{
    fileName = path;
    std::string fullPath = dirPath + fileName;
    std::ifstream ifs(fullPath);
    if (!ifs) {
        Logger::Log("Failed to open JSON file: " + path);
        return false;
    }

    nlohmann::json json;
    ifs >> json;

    // JSON 内のキーに基づいて自動的に Register を呼び出す
    for (auto& [key, value] : json.items()) {
        // すでに登録されていない場合のみ動的に登録
        if (getters_.find(key) == getters_.end()) {
            if (value.is_number_float()) {
                float* floatVal = new float(value.get<float>());
                Register(key, floatVal);
            }
            else if (value.is_number_integer()) {
                int* intVal = new int(value.get<int>());
                Register(key, intVal);
            }
            else if (value.is_string()) {
                std::string* stringVal = new std::string(value.get<std::string>());
                Register(key, stringVal);
            }
            else if (value.is_array() && value.size() > 0 && value[0].is_object()) {
                // 例えば Vector3 の配列を想定している場合
                std::vector<Vector3>* vector3List = new std::vector<Vector3>();
                for (auto& item : value) {
                    Vector3 v = item.get<Vector3>();
                    vector3List->push_back(v);
                }
                Register(key, vector3List);
            }
            // 他の型にも対応する場合はここに追加
            else {
                Logger::Log("Unsupported type for key: " + key);
            }
        }

        // 値をセット
        if (setters_.find(key) != setters_.end()) {
            setters_[key](value);
        }
    }

    return true;
}

bool JsonEditableBase::SaveJson(const std::string& path) const
{
	std::string fullPath = dirPath + fileName;
    nlohmann::json json;
    for (auto& [key, getter] : getters_) {
        json[key] = getter();
    }
    std::ofstream ofs(fullPath);
	if (!ofs) {
		Logger::Log("Failed to open JSON file for writing: " + path);
		return false;
	}

    ofs << json.dump(4);
	return true;
}

void JsonEditableBase::DrawImGui()
{
    ImGui::SeparatorText("Settings");
    for (auto& [name, drawer] : drawers_)
    {
        drawer();
    }
}

void JsonEditableBase::DrawOptions()
{
	ImGui::SeparatorText("Options");
	if (ImGui::Button("Save Json"))
	{
		SaveJson(fileName);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Json"))
	{
		LoadJson(fileName);
	}
}
