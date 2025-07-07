#pragma once
#include <nlohmann/json.hpp>

#include "base/GraphicsTypes.h"

// free 関数版
inline void to_json(nlohmann::json& j, Transform const& t)
{
    // 実際のメンバー名に合わせて書き換えてください
    j = {
      {"translate", t.translate},
      {"rotate", t.rotate},
      {"scale", t.scale}
    };
}

inline void from_json(nlohmann::json const& j, Transform& t)
{
    j.at("translate").get_to(t.translate);
    j.at("rotate").get_to(t.rotate);
    j.at("scale").get_to(t.scale);
}
