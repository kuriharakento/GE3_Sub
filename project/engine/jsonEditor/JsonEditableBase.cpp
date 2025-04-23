#include "JsonEditableBase.h"

#include "base/Logger.h"

void JsonEditableBase::LoadJson(const std::string& path)
{
	fileName = path;
	std::string fullPath = dirPath + fileName;
    std::ifstream ifs(fullPath);
    if (!ifs) {
        Logger::Log("Failed to open JSON file: " + path);
        return;
    }
    nlohmann::json json;
    ifs >> json;

    for (auto& [key, setter] : setters_) {
        if (json.contains(key)) {
            setter(json[key]);
        }
    }
}

void JsonEditableBase::SaveJson(const std::string& path) const
{
	std::string fullPath = dirPath + fileName;
    nlohmann::json json;
    for (auto& [key, getter] : getters_) {
        json[key] = getter();
    }
    std::ofstream ofs(fullPath);
    ofs << json.dump(4);
}

void JsonEditableBase::DrawImGui()
{
    if (ImGui::Button("Save Json"))
    {
		SaveJson(fileName);
    }
	ImGui::SameLine();
    if (ImGui::Button("Load Json"))
    {
		LoadJson(fileName);
    }
    for (auto& [name, drawer] : drawers_)
    {
        drawer();
    }
}