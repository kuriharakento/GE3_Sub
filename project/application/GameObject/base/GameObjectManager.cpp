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
	for (GameObject* gameObject : gameObjects_)
	{
		if (gameObject)
		{
			gameObject->Update();
		}
	}
}

void GameObjectManager::Draw(CameraManager* camera)
{
	for (GameObject* gameObject : gameObjects_)
	{
		if (gameObject)
		{
			gameObject->Draw(camera);
		}
	}
}

void GameObjectManager::Register(GameObject* gameObject)
{
	if(gameObject)
	{
		// ポインタがnullptrでないことを確認
		gameObjects_.push_back(gameObject);
	}
}

void GameObjectManager::Unregister(GameObject* gameObject)
{
	// ポインタをリストから削除
	auto it = std::remove(gameObjects_.begin(), gameObjects_.end(), gameObject);
	if (it != gameObjects_.end())
	{
		gameObjects_.erase(it, gameObjects_.end());
	}
}


