#include "SceneManager.h"

SceneManager::~SceneManager()
{
	//現在のシーンを終了
	currentScene_->Finalize();
	//現在のシーンを解放
	delete currentScene_;
}

void SceneManager::Initialize()
{
	//今のところ必要ない
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
