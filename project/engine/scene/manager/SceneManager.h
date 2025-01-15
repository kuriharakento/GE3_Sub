#pragma once

#include <memory>
#include <string>

#include "engine/scene/interface/BaseScene.h"
#include "scene/SceneContext.h"

class SceneFactory;

class SceneManager
{
public: //メンバ関数
	//デストラクタ
	~SceneManager();
	//コンストラクタ
	SceneManager(SceneFactory* sceneFactory) : currentScene_(nullptr), nextScene_(nullptr), sceneFactory_(sceneFactory) {}

	//初期化
	void Initialize(const SceneContext& context);
	//更新
	void Update();
	//描画
	void Draw3D();
	void Draw2D();

	//シーンの変更
	void ChangeScene(const std::string& sceneName);

public: //アクセッサ
	//カメラマネージャーの取得
	CameraManager* GetCameraManager() { return context_.cameraManager; }
	//スプライト共通部の取得
	SpriteCommon* GetSpriteCommon() { return context_.spriteCommon; }
	//3Dオブジェクト共通部の取得
	Object3dCommon* GetObject3dCommon() { return context_.object3dCommon; }

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

	//シーンコンテキスト
	SceneContext context_;
};

