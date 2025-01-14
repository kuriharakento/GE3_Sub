#include "SceneManager.h"
#include "application/scene/factory/SceneFactory.h"
#include <assert.h>

SceneManager::~SceneManager()
{
	//現在のシーンを終了
	currentScene_->Finalize();
}

void SceneManager::Initialize()
{
	//最初のシーンを生成
	currentScene_.reset(sceneFactory_->CreateScene("TITLE"));
	currentScene_->SetSceneManager(this);
	currentScene_->Initialize();
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
	nextScene_.reset(sceneFactory_->CreateScene(sceneName));
}

void SceneManager::ReserveNextScene()
{
	//次のシーンが予約されているなら
	if (nextScene_)
	{
		//現在のシーンを終了
		currentScene_->Finalize();
		currentScene_.reset();

		//シーンを切り替え
		currentScene_ = std::move(nextScene_);
		nextScene_.reset();
		//次のシーンを初期化
		currentScene_->SetSceneManager(this);
		currentScene_->Initialize();
	}
}
