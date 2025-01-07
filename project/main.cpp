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
#include "application/Manager/BuildingManager.h"
#include "application/Scene/SceneManager.h"
#include "audio/AudioManager.h"
#include "base/Logger.h"
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
	imguiManager->Initilize(winApp, dxCommon, srvManager.get());

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxCommon, srvManager.get());

	//スプライト共通部の初期化
	SpriteCommon* spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	//3Dオブジェクト共通部の初期化
	Object3dCommon* objectCommon = new Object3dCommon();
	objectCommon->Initialize(dxCommon);

	//3Dモデルマネージャーの初期化
	ModelManager::GetInstance()->Initialize(dxCommon);

	//オーディオマネージャーの初期化
	AudioManager::GetInstance()->Initialize();
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
	TextureManager::GetInstance()->LoadTexture("./Resources/black.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/reload.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/reloading.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/gameClear.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/gameOver.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/HP.png");
#pragma endregion

#pragma region モデルの読み込み
	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	ModelManager::GetInstance()->LoadModel("player.obj");
	ModelManager::GetInstance()->LoadModel("enemy.obj");
	ModelManager::GetInstance()->LoadModel("missile.obj");
	ModelManager::GetInstance()->LoadModel("bullet.obj");
	ModelManager::GetInstance()->LoadModel("Building.obj");
	ModelManager::GetInstance()->LoadModel("gameName.obj");
	ModelManager::GetInstance()->LoadModel("skydome.obj");
	ModelManager::GetInstance()->LoadModel("ground.obj");
#pragma endregion

#pragma region 音声ファイルの読み込み
	// 音声ファイルの読み込み
	AudioManager::GetInstance()->LoadWavFile(L"pa", L"pa.wav");
	AudioManager::GetInstance()->PlayMusic(L"pa", true);
	AudioManager::GetInstance()->SetVolume(L"pa", 1.0f);
#pragma endregion

#pragma region 宣言と初期化
	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>();
	sceneManager->Initialize(objectCommon, cameraManager.get(),spriteCommon);

	// カーソル表示状態を管理する変数
	static bool showCursor = true;
	static bool prevShowCursor = true;
#pragma endregion

	///////////////////////////////////////////////////////////////////////
	///																	///
	///						>>>メインループ<<<							///
	///																	///
	///////////////////////////////////////////////////////////////////////

	//メインループの開始
	Logger::Log("\n/===== Start Main Loop!!! =====/\n\n");

	// 最初のシーンを生成
	sceneManager->ChangeScene("TitleScene");

	// カーソルを非表示
	while (ShowCursor(FALSE) >= 0);

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
		if (ImGui::Checkbox("Show Mouse Cursor", &showCursor))
		{
			// カーソルの表示状態が変化したとき
			if (showCursor != prevShowCursor)
			{
				if (showCursor)
				{
					// カーソルを表示
					while (ShowCursor(TRUE) < 0);
				} else
				{
					// カーソルを非表示
					while (ShowCursor(FALSE) >= 0);
				}
				prevShowCursor = showCursor;
			}
		}
		ImGui::Text("Scene: %s", sceneManager->GetCurrentSceneName().c_str());
		if (ImGui::Button("TitleScene"))
		{
			sceneManager->ChangeScene("TitleScene");
		}
		if (ImGui::Button("GameScene"))
		{
			sceneManager->ChangeScene("GameScene");
		}
		if (ImGui::Button("GameClearScene"))
		{
			sceneManager->ChangeScene("GameClearScene");
		}
		if (ImGui::Button("GameOverScene"))
		{
			sceneManager->ChangeScene("GameOverScene");
		}
		{

		}
		ImGui::End();
		#pragma endregion

#endif

		//シーンの更新
		sceneManager->Update();

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

		sceneManager->Draw3D();

		/*--------------[ スプライトの描画 ]-----------------*/

		//スプライトの描画準備。共通の設定を行う
		spriteCommon->CommonRenderingSetting();

		sceneManager->Draw2D();

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

	sceneManager.reset();

	///////////////////////////////////
	///		>>>基盤システム<<<		///
	///////////////////////////////////

	//NOTE:ここは基本的に触らない
	AudioManager::GetInstance()->Finalize();		//オーディオマネージャーの終了処理
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