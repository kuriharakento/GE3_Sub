#pragma once
#include <string>

class IJsonEditable
{
    virtual void DrawImGui() = 0;
    virtual void LoadFromJson(const std::string& path) = 0;
    virtual void SaveToJson(const std::string& path) const = 0;
    virtual ~IJsonEditable() = default;
};

