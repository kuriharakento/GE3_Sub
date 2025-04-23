#include "JsonEditorManager.h"

#include "imgui/imgui.h"

JsonEditorManager* JsonEditorManager::instance_ = nullptr; // シングルトンインスタンス

JsonEditorManager* JsonEditorManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new JsonEditorManager();
	}
	return instance_;
}

void JsonEditorManager::Initialize()
{
	editors_.clear();
}

void JsonEditorManager::Register(const std::string& name, std::shared_ptr<JsonEditableBase> editor)
{
	editors_[name] = editor;
	selectedItem_ = name; // 最後に登録したものを選択状態にする
}

void JsonEditorManager::RenderEditUI()
{
    ImGui::Begin("JSON Editor");

    if (ImGui::BeginTabBar("EditableTabs"))
    {
        for (const auto& [name, editable] : editors_)
        {
            //NULLチェック
            if (!editable) { continue; }

            if (ImGui::BeginTabItem(name.c_str()))
            {
                // タブがアクティブな間は選択状態にしておく
                selectedItem_ = name;

                // そのオブジェクトの ImGui UI を表示
                editable->DrawImGui();

                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void JsonEditorManager::SaveAll()
{

}
