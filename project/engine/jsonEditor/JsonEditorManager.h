#pragma once
#include "IJsonEditable.h"

class JsonEditorManager
{
public:
    void Register(const std::string& name, std::shared_ptr<IJsonEditable> editor);
    void DrawImGui();
    void SaveAll();
private:
    std::unordered_map<std::string, std::shared_ptr<IJsonEditable>> editors_;

};

