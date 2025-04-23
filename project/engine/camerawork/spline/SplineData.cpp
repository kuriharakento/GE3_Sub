#include "SplineData.h"
#include <fstream>

#include "base/Logger.h"

bool SplineData::LoadJson(const std::string& filePath)
{
	std::string fullpath = "Resources/json/" + filePath;
    std::ifstream file(fullpath);
    if (!file.is_open()) {
		Logger::Log("Failed to open file: " + fullpath);
        return false;
    }

    json j;
    file >> j;

    for (const auto& point : j["controlPoints"]) {
        controlPoints.push_back({ point["x"], point["y"], point["z"] });
    }

    return true;
}

bool SplineData::SaveJson(const std::string& filePath)
{
	std::string fullpath = "Resources/json/" + filePath;
	std::ofstream file(fullpath);
	if (!file.is_open()) {
		return false;
	}
	json j;
	for (const auto& point : controlPoints) {
		j["controlPoints"].push_back({ {"x", point.x}, {"y", point.y}, {"z", point.z} });
	}
	file << j.dump(4); // Pretty print with 4 spaces
	return true;
}
