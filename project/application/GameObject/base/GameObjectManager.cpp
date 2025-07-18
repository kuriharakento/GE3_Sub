#include "GameObjectManager.h"

#include "base/Logger.h"
#include "ImGui/imgui.h"

static GameObjectManager* instance_ = nullptr;

GameObjectManager* GameObjectManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new GameObjectManager();
	}
	return instance_;
}

void GameObjectManager::Initialize()
{
	// リストの初期化
	gameObjects_.clear();
}

void GameObjectManager::Finalize()
{
	// 全てのGameObjectを削除
	gameObjects_.clear();

	// インスタンスを削除
	delete instance_;
	instance_ = nullptr;
}

void GameObjectManager::Update()
{
//#ifdef _DEBUG
//	ImGui::Begin("GameObject Manager");
//	ImGui::Text("GameObject Count: %zu", gameObjects_.size());
//	for (size_t i = 0; i < gameObjects_.size(); ++i)
//	{
//		ImGui::PushID(i);
//		if (ImGui::CollapsingHeader(std::to_string(i).c_str()))
//		{
//			ImGui::Text("Tag: %s", gameObjects_[i]->GetTag().c_str());
//			Vector3 position = gameObjects_[i]->GetPosition();
//			gameObjects_[i]->SetPosition(
//				ImGui::DragFloat3("Position", &position.x, 0.1f) ? position : gameObjects_[i]->GetPosition()
//			);
//			Vector3 rotation = gameObjects_[i]->GetRotation();
//			gameObjects_[i]->SetRotation(
//				ImGui::DragFloat3("Rotation", &rotation.x, 0.1f) ? rotation : gameObjects_[i]->GetRotation()
//			);
//			Vector3 scale = gameObjects_[i]->GetScale();
//			gameObjects_[i]->SetScale(
//				ImGui::DragFloat3("Scale", &scale.x, 0.1f) ? scale : gameObjects_[i]->GetScale()
//			);
//		}
//		ImGui::PopID();
//	}
//	ImGui::End();
//#endif


}

void GameObjectManager::Draw(CameraManager* camera)
{
	
}

void GameObjectManager::Register(GameObject* gameObject)
{
	//if (gameObject == nullptr) return;

	//// 既に登録されているかチェック
	//auto it = std::find(gameObjects_.begin(), gameObjects_.end(), gameObject);
	//if (it != gameObjects_.end()) return;

	//// GameObjectを登録
	//gameObjects_.push_back(gameObject);
}

void GameObjectManager::Unregister(GameObject* gameObject)
{
	//if (gameObject == nullptr) return;

	//// 登録されているのかチェック
	//auto itObj = std::find(gameObjects_.begin(), gameObjects_.end(), gameObject);
	//if (itObj == gameObjects_.end()) return;

	//// 削除予定リストに追加（Update/Draw処理中の安全な削除のため）
	//auto it = std::find(pendingRemoval_.begin(), pendingRemoval_.end(), gameObject);
	//if (it == pendingRemoval_.end())
	//{
	//	pendingRemoval_.push_back(gameObject);
	//}
}
