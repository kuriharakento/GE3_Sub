#include "MyGame.h"

void MyGame::Initialize()
{
	//フレームワークの初期化
	Framework::Initialize();

	//シーンコンテキストの作成
	SceneContext context;
	context = {
		spriteCommon_,
		objectCommon_,
		cameraManager_.get()
	};

	//ゲームの初期化処理
	sceneManager_->Initialize(context);
}

void MyGame::Finalize()
{
	//ゲームの終了処理
	sceneManager_.reset();
	//フレームワークの終了処理
	Framework::Finalize();
}

void MyGame::Update()
{
	//フレームワークの更新処理
	Framework::Update();

	//ゲームの更新処理
	sceneManager_->Update();

	//フレームワークの更新後処理
	Framework::PostUpdate();
}

void MyGame::Draw()
{
	//フレームワークの描画前処理
	Framework::PreDraw();

	/////////////////< 描画ここから >////////////////////

	/*----[ 3Dオブジェクトの描画 ]----*/
	//3D描画用設定
	Framework::Draw3DSetting();

	sceneManager_->Draw3D();
	
	/*----[ スプライトの描画 ]----*/
	//2D描画用設定
	Framework::Draw2DSetting();

	sceneManager_->Draw2D();

	/////////////////< 描画ここまで >////////////////////

	//フレームワークの描画後処理
	Framework::PostDraw();
}
