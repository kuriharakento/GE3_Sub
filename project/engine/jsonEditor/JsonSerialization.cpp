#include "JsonSerialization.h"

void to_json(nlohmann::json& j, Transform const& t)
{
    j = {
      {"translate", t.translate},
      {"rotate", t.rotate},
      {"scale", t.scale}
    };
}

void from_json(nlohmann::json const& j, Transform& t)
{
    if (j.contains("translate"))
    {
        j.at("translate").get_to(t.translate);
    }
    else if (j.contains("position"))
    {
        j.at("position").get_to(t.translate);
    }
    j.at("rotate").get_to(t.rotate);
    j.at("scale").get_to(t.scale);
}

void to_json(nlohmann::json& j, Vector3 const& v)
{
    j = { {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

void from_json(nlohmann::json const& j, Vector3& v)
{
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
    j.at("z").get_to(v.z);
}