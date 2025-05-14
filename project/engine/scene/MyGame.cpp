#include "MyGame.h"

#include <future>
#include <chrono>
#include "3d/ModelManager.h"
#include "base/Logger.h"
#include "manager/ParticleManager.h"
#include "manager/TextureManager.h"

void MyGame::Initialize()
{
	//フレームワークの初期化
	Framework::Initialize();

	//シーンコンテキストの作成
	SceneContext context;
	context = {
		spriteCommon_.get(),
		objectCommon_.get(),
		cameraManager_.get(),
		lightManager_.get(),
		postProcessPass.get(),
	};

	// 処理開始時間を記録
	auto startTime = std::chrono::high_resolution_clock::now();

	// テクスチャとモデルの読み込みを並列に実行
	auto loadTexturesFuture = std::async(std::launch::async, [this]() {
		// テクスチャの読み込み
		LoadTextures();
	});

	auto loadModelsFuture = std::async(std::launch::async, [this]() {
		// モデルの読み込み
		LoadModels();
	});

	// 非同期タスクの完了を待つ
	loadTexturesFuture.get();
	loadModelsFuture.get();

	// 処理終了時間を記録
	auto endTime = std::chrono::high_resolution_clock::now();

	// 処理時間を計算
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	// 処理時間をログに出力
	std::stringstream ss;
	ss << "Load Resources completed in " << duration << " milliseconds.\n";
	Logger::Log(ss.str());

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

	//パフォーマンス情報の表示
	Framework::ShowPerformanceInfo();

	//ゲームの更新処理
	sceneManager_->Update();

	//フレームワークの更新後処理
	Framework::PostUpdate();
}

void MyGame::Draw()
{
	/*----[ オフスクリーン描画 ]----*/

	renderTexture_->BeginRender();

	srvManager_->PreDraw();

	/////////////////< 描画ここから >////////////////////

	// ---------- 3D描画 ---------

	

	//3D描画用設定
	Framework::Draw3DSetting();

	//3Dオブジェクトの描画
	sceneManager_->Draw3D();

	//ラインの描画
	LineManager::GetInstance()->RenderLines();

	//パーティクルの描画
	ParticleManager::GetInstance()->Draw();
	
	// ---------- 2D描画 ---------

	//2D描画用設定
	Framework::Draw2DSetting();

	//スプライトの描画
	sceneManager_->Draw2D();

	/////////////////< 描画ここまで >////////////////////

	renderTexture_->EndRender();

	/*----[ スワップチェインの描画 ]----*/

	dxCommon_->PreDraw();

	postProcessPass->Draw(renderTexture_->GetGPUHandle());

#ifdef _DEBUG
	//ImGuiの描画
	imguiManager_->Draw();
#endif

	dxCommon_->PostDraw();

	
}

void MyGame::LoadTextures()
{
	TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/black.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/testSprite.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/monsterBall.png");
}

void MyGame::LoadModels()
{
	ModelManager::GetInstance()->LoadModel("cube.obj");
	ModelManager::GetInstance()->LoadModel("highPolygonSphere.obj");
	ModelManager::GetInstance()->LoadModel("terrain.obj");
	ModelManager::GetInstance()->LoadModel("plane.gltf");
}
