#include "SceneManager.h"

#include <assert.h>

SceneManager::~SceneManager()
{
	//現在のシーンを終了
	currentScene_->Finalize();
	//現在のシーンを解放
	delete currentScene_;
}

void SceneManager::Initialize()
{
	//
}

void SceneManager::Update()
{
	//次のシーンが予約されているか
	ReserveNextScene();

	//シーンの更新
	currentScene_->Update();
}

void SceneManager::Draw3D()
{
	currentScene_->Draw3D();
}

void SceneManager::Draw2D()
{
	currentScene_->Draw2D();
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	//nullチェック
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	//次のシーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

void SceneManager::ReserveNextScene()
{
	//次のシーンが予約されているなら
	if (nextScene_)
	{
		//現在のシーンを終了
		currentScene_->Finalize();
		delete currentScene_;

		//シーンを切り替え
		currentScene_ = nextScene_;
		nextScene_ = nullptr;
		//次のシーンを初期化
		currentScene_->Initialize();

	}
}
