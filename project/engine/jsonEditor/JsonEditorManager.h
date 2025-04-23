#pragma once
#include <memory>
#include <unordered_map>

#include "JsonEditableBase.h"

class JsonEditorManager
{
public:
	static JsonEditorManager* GetInstance();
	void Initialize();
    void Register(const std::string& name, std::shared_ptr<JsonEditableBase> editor);
	void Edit();
    void SaveAll();

private:
    std::unordered_map<std::string, std::shared_ptr<JsonEditableBase>> editors_;
	std::string selectedItem_;

private: //シングルトンインスタンス
	static JsonEditorManager* instance_;
	JsonEditorManager() = default;
	~JsonEditorManager() = default;
	JsonEditorManager(const JsonEditorManager&) = delete;
	JsonEditorManager& operator=(const JsonEditorManager&) = delete;
};
