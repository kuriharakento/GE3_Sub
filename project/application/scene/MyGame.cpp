#include "MyGame.h"

void MyGame::Initialize()
{
	//フレームワークの初期化
	Framework::Initialize();

	//ゲームの初期化処理

}

void MyGame::Finalize()
{
	//ゲームの終了処理


	//フレームワークの終了処理
	Framework::Finalize();
}

void MyGame::Update()
{
	//フレームワークの更新処理
	Framework::Update();

	/////////////////< 更新処理ここから >////////////////////



	/////////////////< 更新処理ここまで >////////////////////
	
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

	
	/*----[ スプライトの描画 ]----*/
	//2D描画用設定
	Framework::Draw2DSetting();


	/////////////////< 描画ここまで >////////////////////

	//フレームワークの描画後処理
	Framework::PostDraw();
}
