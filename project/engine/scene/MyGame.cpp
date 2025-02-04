#include "MyGame.h"

#include <future>
#include <chrono>
#include "3d/ModelManager.h"
#include "base/Logger.h"
#include "manager/TextureManager.h"

void MyGame::Initialize()
{
	//フレームワークの初期化
	Framework::Initialize();

	// 処理開始時間を記録
	auto startTime = std::chrono::high_resolution_clock::now();

	// テクスチャとモデルの読み込みを並列に実行
	auto loadTexturesFuture = std::async(std::launch::async, [this]() {
		LoadTextures();
	});
	auto loadModelsFuture = std::async(std::launch::async, [this]() {
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
	ss << "Initialize completed in " << duration << " milliseconds.\n";
	Logger::Log(ss.str());

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

void MyGame::LoadModels()
{
	Logger::Log("LoadModels\n");
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
	Logger::Log("LoadModels Complete!\n");
}

void MyGame::LoadTextures()
{
	Logger::Log("LoadTextures\n");
	TextureManager::GetInstance()->LoadTexture("./Resources/monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/black.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/reload.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/reloading.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/gameClear.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/gameOver.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/HP.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/testSprite.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/clearCondition.png");
	Logger::Log("LoadTextures Complete!\n");
}