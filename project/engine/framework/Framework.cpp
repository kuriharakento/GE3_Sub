#include "Framework.h"

#include "3d/ModelManager.h"
#include "audio/Audio.h"
#include "base/Logger.h"
#include "input/Input.h"
#include "manager/ParticleManager.h"
#include "manager/TextureManager.h"

void Framework::Initialize()
{
	//ウィンドウアプリケーションの初期化
	winApp_ = new WinApp();
	winApp_->Initialize();

	//DirectXCoomonの初期化
	dxCommon_ = new DirectXCommon();
	dxCommon_->Initialize(winApp_);

	//SRVマネージャーの初期化
	srvManager_ = std::make_unique<SrvManager>();
	srvManager_->Initialize(dxCommon_);

	//ImGuiの初期化
	imguiManager_ = new ImGuiManager();
	imguiManager_->Initilize(winApp_, dxCommon_, srvManager_.get());

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxCommon_, srvManager_.get());

	//スプライト共通部の初期化
	spriteCommon_ = new SpriteCommon();
	spriteCommon_->Initialize(dxCommon_);

	//3Dオブジェクト共通部の初期化
	objectCommon_ = new Object3dCommon();
	objectCommon_->Initialize(dxCommon_);

	//3Dモデルマネージャーの初期化
	ModelManager::GetInstance()->Initialize(dxCommon_);

	//パーティクルマネージャーの初期化
	ParticleManager::GetInstance()->Initialize(dxCommon_, srvManager_.get());

	//入力の初期化
	Input::GetInstance()->Initialize(winApp_);

	//オーディオの初期化
	Audio::GetInstance()->Initialize();

	//カメラマネージャーの初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->AddCamera("main");
	cameraManager_->SetActiveCamera("main");
	cameraManager_->GetActiveCamera()->SetTranslate({ 0.0f,4.0f,-10.0f });
	cameraManager_->GetActiveCamera()->SetRotate({ 0.0f,0.0f,0.0f });

	//3Dオブジェクト共通部に初期カメラをセット
	objectCommon_->SetDefaultCamera(cameraManager_->GetActiveCamera());

	//シーンファクトリーの初期化
	sceneFactory_ = std::make_unique<SceneFactory>();

	//シーンマネージャーの初期化
	sceneManager_ = std::make_unique<SceneManager>(sceneFactory_.get());
}

void Framework::Finalize()
{
	//NOTE:ここは基本的に触らない
	sceneManager_.reset();							//シーンマネージャーの解放
	winApp_->Finalize();							//ウィンドウアプリケーションの終了処理
	delete winApp_;									//ウィンドウアプリケーションの解放
	imguiManager_->Finalize();						//ImGuiManagerの終了処理
	delete imguiManager_;							//ImGuiManagerの解放
	TextureManager::GetInstance()->Finalize();		//テクスチャマネージャーの終了処理
	delete dxCommon_;								//DirectXCommonの解放
	delete spriteCommon_;							//スプライト共通部の解放
	delete objectCommon_;							//3Dオブジェクト共通部の解放
	ModelManager::GetInstance()->Finalize();		//3Dモデルマネージャーの終了処理
	ParticleManager::GetInstance()->Finalize();		//パーティクルマネージャーの終了処理
	Input::GetInstance()->Finalize();				//入力の解放
	Audio::GetInstance()->Finalize();				//オーディオの解放
}

void Framework::Update()
{
	//ウィンドウアプリケーションのメッセージ処理
	if (winApp_->ProcessMessage())
	{
		endRequest_ = true;
	}

	//フレームの先頭でImGuiに、ここからフレームが始まる旨を告げる
	imguiManager_->Begin();

	//入力の更新
	Input::GetInstance()->Update();

	//カメラの更新
	cameraManager_->Update();
}

void Framework::PostUpdate()
{
	//ゲームの処理が終わり描画処理に入る前にImGuiの内部コマンドを生成する
	imguiManager_->End();
}

void Framework::Draw3DSetting()
{
	//NOTE:3Dオブジェクトの描画準備。共通の設定を行う
	objectCommon_->CommonRenderingSetting();
}

void Framework::Draw2DSetting()
{
	//スプライトの描画準備。共通の設定を行う
	spriteCommon_->CommonRenderingSetting();
}

void Framework::PreDraw()
{
	//描画前処理
	dxCommon_->PreDraw();
	srvManager_->PreDraw();
}

void Framework::PostDraw()
{
	//ImGuiの描画
	imguiManager_->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}

void Framework::Run()
{
	//初期化
	Initialize();

	//メインループの開始を告げる
	Logger::Log("\n/******* Start Main Loop *******/\n\n");

	while(true)
	{
		//更新
		Update();
		//終了リクエストがあるか
		if (IsEndRequest())
		{
			break;
		}
		//描画
		Draw();
	}
	//メインループの終了を告げる
	Logger::Log("\n/******* End Main Loop *******/\n\n");

	//終了処理
	Finalize();
}
