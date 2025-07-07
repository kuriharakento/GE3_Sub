#pragma once
#pragma once
#include <nlohmann/json.hpp>
#include "math/Vector3.h"

inline void to_json(nlohmann::json& j, Vector3 const& v)
{
    j = { {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

inline void from_json(nlohmann::json const& j, Vector3& v)
{
	j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
    j.at("z").get_to(v.z);
}