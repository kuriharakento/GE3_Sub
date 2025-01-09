#pragma once

#include "application/scene/interface/AbstractSceneFactory.h"
#include "application/scene/interface/IScene.h"

class SceneManager : public IScene
{
public: //メンバ関数
	//デストラクタ
	~SceneManager();
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw3D() override;
	void Draw2D() override;

	void ChangeScene(const std::string& sceneName);

private: //メンバ関数
	//次のシーンが予約されているか
	void ReserveNextScene();

private: //メンバ変数
	//今のシーン
	IScene* currentScene_;
	//次のシーン
	IScene* nextScene_;

	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_;
};

