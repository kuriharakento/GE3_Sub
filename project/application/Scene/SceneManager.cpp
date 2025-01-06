#include "SceneManager.h"

#include "GameClearScene.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "base/Logger.h"
#include "2d/SpriteCommon.h"

void SceneManager::ChangeScene(const std::string& sceneName)
{
	//同じシーンに移動しようとしているときは何もしない
	if (currentSceneName_ == sceneName)
	{
		Logger::Log("The current scene has the same name: " + sceneName);
		return;
	}

	//現在のシーンを削除
	currentScene_.reset();

	//新しいシーンを作成
	if(sceneName == "TitleScene")
	{
		currentScene_ = std::make_unique<TitleScene>(cameraManager_,spriteCommon_);;
	} else if (sceneName == "GameScene")
	{
		currentScene_ = std::make_unique<GameScene>(object3dCommon_, cameraManager_, spriteCommon_);
	} else if (sceneName == "GameClearScene")
	{
		currentScene_ = std::make_unique<GameClearScene>(cameraManager_, spriteCommon_);
	} else if (sceneName == "GameOverScene")
	{
		currentScene_ = std::make_unique<GameOverScene>(cameraManager_,spriteCommon_);
	}else
	{
		Logger::Log("Unknown scene name: " + sceneName);
		return;
	}

	//シーン名を保存
	currentSceneName_ = sceneName;

	//ログにシーン名を出力
	Logger::Log("Now Scene: " + sceneName + "\n");

	//初期化
	currentScene_->Initialize(this);
}

void SceneManager::Initialize(Object3dCommon* objectCommon, CameraManager* camera, SpriteCommon* spriteCommon)
{
	//ポインタを受け取る
	object3dCommon_ = objectCommon;
	cameraManager_ = camera;
	spriteCommon_ = spriteCommon;

	//初期化
	sceneFactories_.clear();
	currentScene_.reset();
	currentSceneName_.clear();
}

void SceneManager::Update()
{
	//シーンが終了していた場合は次のシーンに移行
	NextScene();

	//シーンの更新
    if (currentScene_) {
        currentScene_->Update();
    }
}

// 3D描画
void SceneManager::Draw3D()
{
	if (currentScene_)
	{
		currentScene_->Draw3D();
	}
}

// 2D描画
void SceneManager::Draw2D()
{
	if (currentScene_)
	{
		currentScene_->Draw2D();
	}
}

void SceneManager::NextScene()
{
	//シーンの変更
	if (currentScene_->IsEnd())
	{
		if (currentSceneName_ == "TitleScene")
		{
			ChangeScene("GameScene");
		} else if (currentSceneName_ == "GameScene")
		{
			ChangeScene("GameClearScene");
		} else if (currentSceneName_ == "GameClearScene")
		{
			ChangeScene("TitleScene");
		} else if (currentSceneName_ == "GameOverScene")
		{
			ChangeScene("TitleScene");
		}
	}
}
