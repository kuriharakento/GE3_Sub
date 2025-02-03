#include "Framework.h"

#include "3d/ModelManager.h"
#include "audio/Audio.h"
#include "base/Logger.h"
#include "input/Input.h"
#include "manager/ParticleManager.h"
#include "manager/TextureManager.h"
#include <Psapi.h>

void Framework::Initialize()
{
	//ウィンドウアプリケーションの初期化
	winApp_ = std::make_unique<WinApp>();
	winApp_->Initialize();

	//DirectXCoomonの初期化
	dxCommon_ = std::make_unique<DirectXCommon>();
	dxCommon_->Initialize(winApp_.get());

	//SRVマネージャーの初期化
	srvManager_ = std::make_unique<SrvManager>();
	srvManager_->Initialize(dxCommon_.get());

	//ImGuiの初期化
	imguiManager_ = std::make_unique<ImGuiManager>();
	imguiManager_->Initialize(winApp_.get(), dxCommon_.get(), srvManager_.get());

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get());

	//スプライト共通部の初期化
	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(dxCommon_.get());

	//3Dオブジェクト共通部の初期化
	objectCommon_ = std::make_unique<Object3dCommon>();
	objectCommon_->Initialize(dxCommon_.get());

	//3Dモデルマネージャーの初期化
	ModelManager::GetInstance()->Initialize(dxCommon_.get());

	//パーティクルマネージャーの初期化
	ParticleManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get());

	//入力の初期化
	Input::GetInstance()->Initialize(winApp_.get());

	//オーディオの初期化
	Audio::GetInstance()->Initialize();

	//カメラマネージャーの初期化
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->AddCamera("main");
	cameraManager_->SetActiveCamera("main");
	cameraManager_->GetActiveCamera()->SetTranslate({ 0.0f,1.0f,-10.0f });
	cameraManager_->GetActiveCamera()->SetRotate({ 0.0f,0.0f,0.0f });

	//3Dオブジェクト共通部に初期カメラをセット
	objectCommon_->SetDefaultCamera(cameraManager_->GetActiveCamera());

	//シーンファクトリーの初期化
	sceneFactory_ = std::make_unique<SceneFactory>();

	//シーンマネージャーの初期化
	sceneManager_ = std::make_unique<SceneManager>(sceneFactory_.get());

	//ライトマネージャーの初期化
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize(dxCommon_.get());
}

void Framework::Finalize()
{
	//NOTE:ここは基本的に触らない
	sceneManager_.reset();							//シーンマネージャーの解放
	winApp_->Finalize();							//ウィンドウアプリケーションの終了処理
	winApp_.reset();									//ウィンドウアプリケーションの解放
	imguiManager_->Finalize();						//ImGuiManagerの終了処理
	imguiManager_.reset();							//ImGuiManagerの解放
	TextureManager::GetInstance()->Finalize();		//テクスチャマネージャーの終了処理
	dxCommon_.reset();								//DirectXCommonの解放
	spriteCommon_.reset();							//スプライト共通部の解放
	objectCommon_.reset();							//3Dオブジェクト共通部の解放
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

	//オーディオの更新
	Audio::GetInstance()->Update();

	//ライトマネージャーの更新
	lightManager_->Update();
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

void Framework::ShowPerformanceInfo()
{
	// ウィンドウの位置を左上に固定
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	// ウィンドウのサイズを固定
	ImGui::SetNextWindowSize(ImVec2(200, 65), ImGuiCond_Always);
	ImGui::Begin("Performance",nullptr,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::Text("FPS : %.2f", ImGui::GetIO().Framerate);
	// メモリ使用量
	PROCESS_MEMORY_COUNTERS memInfo;
	GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
	ImGui::Text("Memory Usage : %.2f MB", memInfo.WorkingSetSize / (1024.0f * 1024.0f));
	ImGui::End();
}
