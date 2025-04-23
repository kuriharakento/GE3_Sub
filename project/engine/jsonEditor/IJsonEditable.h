#pragma once
#include <string>

/// JSON編集可能なインターフェース
class IJsonEditable
{
protected:
    virtual void DrawImGui() = 0;
    virtual void LoadJson(const std::string& path) = 0;
    virtual void SaveJson(const std::string& path) const = 0;
    virtual ~IJsonEditable() = default;
};

