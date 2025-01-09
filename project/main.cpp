#include <Windows.h>
#include <vector>

//ImGui
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

///////////////////////////////////////////////////////////////////////
///					>>>自作クラスのインクルード<<<						///
///////////////////////////////////////////////////////////////////////

#pragma region 自作クラスのインクルード
#include "input/Input.h"
#include "base/WinApp.h"
#include "base/DirectXCommon.h"
#include "base/D3DResourceLeakChecker.h"
#include "2d/SpriteCommon.h"
#include "2d/Sprite.h"
#include "manager/TextureManager.h"
#include "3d/Object3dCommon.h"
#include "3d/Object3d.h"
#include "3d/ModelManager.h"
#include "base/Logger.h"
#include "manager/CameraManager.h"
#include "manager/ImGuiManager.h"
#include "manager/SrvManager.h"
#include "effects/ParticleEmitter.h"
#include "framework/MyGame.h"
#include "math/VectorFunc.h"
#pragma endregion

//コードを整理するときに使う
/*--------------[  ]-----------------*/

///////////////////////////////////////////////////////////////////////
///							>>>構造体の宣言<<<						///
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
///						>>>関数の宣言<<<								///
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
///						>>>グローバル変数の宣言<<<						///
///////////////////////////////////////////////////////////////////////

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ゲームの初期化
	std::unique_ptr<MyGame> game = std::make_unique<MyGame>();
	game->Initialize();

	///////////////////////////////////////////////////////////////////////
	///						>>>変数の宣言<<<								///
	///////////////////////////////////////////////////////////////////////

	#pragma region テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture("./Resources/monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");
	#pragma endregion

	#pragma region モデルの読み込み
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	#pragma endregion


	/////////////////////////////////////////////////////////////////////// ///
	///						>>>メインループ<<<							///
	///////////////////////////////////////////////////////////////////////

	//メインループ開始
	Logger::Log("/*==== Start Main Loop!!! ====*/");

	//マウスカーソルの表示,マウスカーソルを非表示にする場合は、ShowCursor(false)を使う
	while (ShowCursor(true) < 0);

	//ゲームループ
	while (true)
	{
		//ゲームの更新
		game->Update();
		//終了リクエストが来たらループを抜ける
		if (game->IsEndRequest())
		{
			break;
		}
		//ゲームの描画
		game->Draw();
	}
	//ゲームの終了
	game->Finalize();
	
	return 0;
}