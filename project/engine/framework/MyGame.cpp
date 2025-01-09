#include "MyGame.h"

#include "3d/ModelManager.h"
#include "input/Input.h"
#include "manager/ParticleManager.h"
#include "manager/TextureManager.h"

void MyGame::Initialize()
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
	ParticleManager::GetInstance()->Initialize(dxCommon_,srvManager_.get());

	//入力の初期化
	Input::GetInstance()->Initialize(winApp_);

	//カメラマネージャーの初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->AddCamera("main");
	cameraManager_->SetActiveCamera("main");
	cameraManager_->GetActiveCamera()->SetTranslate({ 0.0f,4.0f,-10.0f });
	cameraManager_->GetActiveCamera()->SetRotate({ 0.0f,0.0f,0.0f });

	//3Dオブジェクト共通部に初期カメラをセット
	objectCommon_->SetDefaultCamera(cameraManager_->GetActiveCamera());
}

void MyGame::Finalize()
{
	//NOTE:ここは基本的に触らない
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
}

void MyGame::Update()
{
	//windowsノメッセージ処理
	if (winApp_->ProcessMessage())
	{
		//ゲームループを抜ける
		endRequest_ = true;
	}

	//フレームの先頭でImGuiに、ここからフレームが始まる旨を告げる
	imguiManager_->Begin();

	//入力の更新
	Input::GetInstance()->Update();

	//カメラの更新
	cameraManager_->Update();

	//ゲームの処理が終わり描画処理に入る前にImGuiの内部コマンドを生成する
	imguiManager_->End();
}

void MyGame::Draw()
{
	//描画前処理
	dxCommon_->PreDraw();
	srvManager_->PreDraw();

	/*--------------[ 3Dオブジェクトの描画 ]-----------------*/

	//NOTE:3Dオブジェクトの描画準備。共通の設定を行う
	objectCommon_->CommonRenderingSetting();


	/*--------------[ スプライトの描画 ]-----------------*/

	//スプライトの描画準備。共通の設定を行う
	spriteCommon_->CommonRenderingSetting();


	//NOTE:ここから下には描画処理は書かない
	//ImGuiの描画
	imguiManager_->Draw();
	//描画後処理
	dxCommon_->PostDraw();
}
