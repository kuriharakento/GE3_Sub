#include "SceneManager.h"

#include "GameScene.h"
#include "TitleScene.h"
#include "base/Logger.h"

void SceneManager::ChangeScene(const std::string& sceneName)
{
	//現在のシーンを削除
	currentScene_.reset();

	//新しいシーンを作成
	if(sceneName == "TitleScene")
	{
		currentScene_ = std::make_unique<TitleScene>(cameraManager_);
		Logger::Log("Now Scene: TitleScen\n");
	} else if (sceneName == "GameScene")
	{
		currentScene_ = std::make_unique<GameScene>(object3dCommon_, cameraManager_);
		Logger::Log("Now Scene: GameScene\n");
	} else
	{
		Logger::Log("Unknown scene name: " + sceneName);
		assert(false);
	}

	//初期化
	currentScene_->Initialize(this);
}

void SceneManager::Initialize(Object3dCommon* objectCommon, CameraManager* camera)
{
	//ポインタを受け取る
	object3dCommon_ = objectCommon;
	cameraManager_ = camera;

	//初期化
	sceneFactories_.clear();
	currentScene_.reset();
	currentSceneName_.clear();
}

void SceneManager::Update()
{
    if (currentScene_) {
        currentScene_->Update();
    }
}

void SceneManager::Draw3D()
{
	if (currentScene_)
	{
		currentScene_->Draw3D();
	}
}

void SceneManager::Draw2D()
{
	if (currentScene_)
	{
		currentScene_->Draw2D();
	}
}
