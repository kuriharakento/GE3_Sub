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
#include "manager/CameraManager.h"
#include "manager/ImGuiManager.h"
#include "manager/SrvManager.h"
#include "math/VectorFunc.h"
#include "application/Entities/Player.h"
#include "application/Manager/CollisionManager.h"
#include "application/Manager/EnemyManager.h"
#pragma endregion

//コードを整理するときに使う
/*--------------[  ]-----------------*/

///////////////////////////////////////////////////////////////////////
///							>>>構造体の宣言<<<						///
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
///						>>>関数の宣言<<<								///
///////////////////////////////////////////////////////////////////////

void AddCollisions(CollisionManager* collisionManager, Player* player, EnemyManager* enemyManager) {
	collisionManager->Clear();
	collisionManager->AddCollidable(player);
	ICollidable* enemyCollidable;
	for (int i = 0; i < enemyManager->GetEnemies().size(); i++)
	{
		enemyCollidable = enemyManager->GetEnemy(i);
		collisionManager->AddCollidable(enemyCollidable);
	}
}

///////////////////////////////////////////////////////////////////////
///						>>>グローバル変数の宣言<<<						///
///////////////////////////////////////////////////////////////////////


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	D3DResourceLeakChecker leakCheck;

	///////////////////////////////////////////////////////////////////////
	///						>>>基盤システム初期化<<<						///
	///////////////////////////////////////////////////////////////////////

	#pragma region 基盤システム初期化
	//ウィンドウアプリケーションの初期化
	WinApp* winApp = new WinApp();
	winApp->Initialize();

	//DirectXCoomonの初期化
	DirectXCommon* dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//SRVマネージャーの初期化
	std::unique_ptr<SrvManager> srvManager = std::make_unique<SrvManager>();
	srvManager->Initialize(dxCommon);

	//ImGuiの初期化
	ImGuiManager* imguiManager = new ImGuiManager();
	imguiManager->Initilize(winApp,dxCommon,srvManager.get());

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxCommon,srvManager.get());

	//スプライト共通部の初期化
	SpriteCommon* spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	//3Dオブジェクト共通部の初期化
	Object3dCommon* objectCommon = new Object3dCommon();
	objectCommon->Initialize(dxCommon);

	//3Dモデルマネージャーの初期化
	ModelManager::GetInstance()->Initialize(dxCommon);

	#pragma endregion

	///////////////////////////////////////////////////////////////////////
	///						>>>汎用機能初期化<<<							///
	///////////////////////////////////////////////////////////////////////

	//入力の初期化
	Input::GetInstance()->Initialize(winApp);

	//カメラマネージャーの初期化
	std::unique_ptr<CameraManager> cameraManager = std::make_unique<CameraManager>();
	cameraManager->AddCamera("main");
	cameraManager->SetActiveCamera("main");
	cameraManager->GetActiveCamera()->SetTranslate({ 0.0f,2.0f,-7.0f });
	cameraManager->GetActiveCamera()->SetRotate({ 0.0f,0.0f,0.0f });

	//3Dオブジェクト共通部にカメラを設定
	objectCommon->SetDefaultCamera(cameraManager->GetActiveCamera());

	///////////////////////////////////////////////////////////////////////
	///						>>>変数の宣言<<<								///
	///////////////////////////////////////////////////////////////////////


	#pragma region テクスチャの読み込み
	//テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture("./Resources/monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");
	#pragma endregion

	#pragma region モデルの読み込み
	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	#pragma endregion

	#pragma region 宣言と初期化
	/*===[ プレイヤー ]===*/
	std::unique_ptr<Player> player = std::make_unique<Player>();
	player->Initialize("plane.obj", objectCommon,cameraManager.get());
	/*===[ 敵 ]===*/
	std::unique_ptr<EnemyManager> enemyManager = std::make_unique<EnemyManager>();
	enemyManager->Initialize(objectCommon, cameraManager.get(), player.get(), "plane.obj");
	/*===[ 当たり判定マネージャー ]===*/
	std::unique_ptr<CollisionManager> collisionManager = std::make_unique<CollisionManager>();
	collisionManager->Initialize();
	#pragma endregion

	///////////////////////////////////////////////////////////////////////
	///																	///
	///						>>>メインループ<<<							///
	///																	///
	///////////////////////////////////////////////////////////////////////

	//ゲームループ
	while (true)
	{
		//windowsノメッセージ処理
		if (winApp->ProcessMessage())
		{
			//ゲームループを抜ける
			break;
		}

		//フレームの先頭でImGuiに、ここからフレームが始まる旨を告げる
		imguiManager->Begin();

		///////////////////////////////////////////
		///		>>>汎用機能の更新ここから<<<		///
		///////////////////////////////////////////
		
		//入力の更新
		Input::GetInstance()->Update();

		//カメラの更新
		cameraManager->Update();

		///////////////////////////////////////////
		///		>>>汎用機能の更新ここまで<<<		///
		///////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		///						>>>更新処理ここから<<<							///
		///////////////////////////////////////////////////////////////////////
		
#ifdef _DEBUG
		/*--------------[ ImGui ]-----------------*/

		#pragma region シーン全体の設定
		ImGui::Begin("Settings");

		ImGui::End();
		#pragma endregion

#endif

		//プレイヤーの更新
		player->Update();

		//敵の更新
		enemyManager->Update();

		//当たり判定の追加
		AddCollisions(collisionManager.get(), player.get(), enemyManager.get());
		//当たり判定の更新
		collisionManager->Update();

		///////////////////////////////////////////////////////////////////////
		///						>>>更新処理ここまで<<<							///
		///////////////////////////////////////////////////////////////////////

		//ゲームの処理が終わり描画処理に入る前にImGuiの内部コマンドを生成する
		imguiManager->End();

		///////////////////////////////////////////////////////////////////////
		///						>>>描画処理ここから<<<							///
		///////////////////////////////////////////////////////////////////////

		//描画前処理
		dxCommon->PreDraw();
		srvManager->PreDraw();
		

		/*--------------[ 3Dオブジェクトの描画 ]-----------------*/

		//NOTE:3Dオブジェクトの描画準備。共通の設定を行う
		objectCommon->CommonRenderingSetting();

		//プレイヤーの描画
		player->Draw();

		//敵の描画
		enemyManager->Draw();

		/*--------------[ スプライトの描画 ]-----------------*/

		//スプライトの描画準備。共通の設定を行う
		spriteCommon->CommonRenderingSetting();

		/*--------------[ 汎用機能の処理 ]-----------------*/

		//ImGuiの描画
		imguiManager->Draw();

		//描画後処理
		dxCommon->PostDraw();

		///////////////////////////////////////////////////////////////////////
		///						>>>描画処理ここまで<<<							///
		///////////////////////////////////////////////////////////////////////
	}

	///////////////////////////////////////////////////////////////////////
	///						>>>解放処理ここから<<<							///
	///////////////////////////////////////////////////////////////////////
		
	/*--------------[ スプライトの解放 ]-----------------*/



	/*--------------[ オブジェクトの解放 ]-----------------*/



	///////////////////////////////////
	///		>>>基盤システム<<<		///
	///////////////////////////////////

	//NOTE:ここは基本的に触らない
	winApp->Finalize();								//ウィンドウアプリケーションの終了処理
	delete winApp;									//ウィンドウアプリケーションの解放
	imguiManager->Finalize();						//ImGuiManagerの終了処理
	delete imguiManager;							//ImGuiManagerの解放
	TextureManager::GetInstance()->Finalize();		//テクスチャマネージャーの終了処理
	delete dxCommon;								//DirectXCommonの解放
	delete spriteCommon;							//スプライト共通部の解放
	delete objectCommon;							//3Dオブジェクト共通部の解放
	ModelManager::GetInstance()->Finalize();		//3Dモデルマネージャーの終了処理
	Input::GetInstance()->Finalize();				//入力の解放

	return 0;
}