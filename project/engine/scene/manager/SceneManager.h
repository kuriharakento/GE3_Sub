#pragma once

#include <memory>
#include <string>

#include "engine/scene/interface/BaseScene.h"

class SceneFactory;

class SceneManager
{
public: //メンバ関数
	//デストラクタ
	~SceneManager();
	//コンストラクタ
	SceneManager(SceneFactory* sceneFactory) : currentScene_(nullptr), nextScene_(nullptr), sceneFactory_(sceneFactory) {}

	//初期化
	void Initialize();;
	//更新
	void Update();
	//描画
	void Draw3D();
	void Draw2D();

	//シーンの変更
	void ChangeScene(const std::string& sceneName);

private: //メンバ関数
	//次のシーンが予約されているか
	void ReserveNextScene();

private: //メンバ変数
	//今のシーン
	std::unique_ptr<BaseScene> currentScene_;
	//次のシーン
	std::unique_ptr<BaseScene> nextScene_;

	//シーンファクトリー
	SceneFactory* sceneFactory_;

	//シーンの名前
	std::string currentSceneName_ = "";
	std::string nextSceneName_ = "";
};

