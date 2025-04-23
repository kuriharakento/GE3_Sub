#pragma once
#include <nlohmann/json.hpp>
#include "math/Vector3.h"

// Vector3 を JSON に変換
inline void to_json(nlohmann::json& j, const Vector3& vec) {
    j = nlohmann::json{ {"x", vec.x}, {"y", vec.y}, {"z", vec.z} };
}

// JSON を Vector3 に変換
inline void from_json(const nlohmann::json& j, Vector3& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}