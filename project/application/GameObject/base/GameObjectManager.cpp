#include "GameObjectManager.h"

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
	//// 削除予定のGameObjectを削除
	//for (auto* gameObject : pendingRemoval_)
	//{
	//	auto it = std::find(gameObjects_.begin(), gameObjects_.end(), gameObject);
	//	if (it != gameObjects_.end())
	//	{
	//		gameObjects_.erase(it);
	//	}
	//}
	//pendingRemoval_.clear();

	//// 全てのGameObjectを更新
	//for (const auto& gameObject : gameObjects_)
	//{
	//	if (gameObject && gameObject->IsActive()) // アクティブなGameObjectのみ更新
	//	{
	//		gameObject->Update(); // 各GameObjectの更新
	//	}
	//}
}

void GameObjectManager::Draw(CameraManager* camera)
{
	//for (const auto& gameObject : gameObjects_)
	//{
	//	if (gameObject && gameObject->IsActive()) // アクティブなGameObjectのみ描画
	//	{
	//		gameObject->Draw(camera); // 各GameObjectの描画
	//	}
	//}
}

void GameObjectManager::Register(GameObject* gameObject)
{
	if (gameObject == nullptr) return;

	// 既に登録されているかチェック
	auto it = std::find(gameObjects_.begin(), gameObjects_.end(), gameObject);
	if (it != gameObjects_.end()) return;

	// GameObjectを登録
	gameObjects_.push_back(gameObject);
}

void GameObjectManager::Unregister(GameObject* gameObject)
{
	if (gameObject == nullptr) return;

	// 削除予定リストに追加（Update/Draw処理中の安全な削除のため）
	auto it = std::find(pendingRemoval_.begin(), pendingRemoval_.end(), gameObject);
	if (it == pendingRemoval_.end())
	{
		pendingRemoval_.push_back(gameObject);
	}
}
