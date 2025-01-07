#include "TitleScene.h"
#include "SceneManager.h"
#include "input/Input.h"
#include "2d/SpriteCommon.h"

TitleScene::~TitleScene()
{
	buildingManager_.reset();
}

void TitleScene::Initialize(SceneManager* sceneManager)
{
	sceneManager_ = sceneManager;

	//カメラを作成しアクティブにする
	cameraManager_->AddCamera("TitleCamera");
	cameraManager_->SetActiveCamera("TitleCamera");
	cameraManager_->GetActiveCamera()->SetTranslate({ 0.0f,1.0f,-5.0f });

	//スライド
	slide_ = std::make_unique<Slide>();
	slide_->Initialize(spriteCommon_);

	//スライドの最初の状態を設定
	slide_->Start(Slide::Status::SlideOutFromBothSides, 1.0f);

	//ゲーム名のオブジェクトの初期化
	gameName_ = std::make_unique<Object3d>();
	gameName_->Initialize(object3dCommon_);
	gameName_->SetModel("gameName.obj");
	gameName_->SetTranslate({ 0.0f, 1.3f, 0.0f });
	gameName_->SetScale({0.5f,0.5f,0.5f });

	//初期位置
	initialPosition_ = gameName_->GetTranslate();

	//天球の初期化
	skyDome_ = std::make_unique<Object3d>();
	skyDome_->Initialize(object3dCommon_);
	skyDome_->SetModel("skydome.obj");

	//地面の初期化
	ground_ = std::make_unique<Object3d>();
	ground_->Initialize(object3dCommon_);
	ground_->SetModel("ground.obj");

	//ビルの初期化
	buildingManager_ = std::make_unique<BuildingManager>();;
	buildingManager_->Initialize(object3dCommon_, cameraManager_);
	buildingManager_->GenerateBuilding(60, 15.0f,60.0f);
	
}

void TitleScene::Update()
{
	switch(currentPhase_)
	{
	case ScenePhase::Start:
		slide_->Update();
		if (slide_->IsFinish())
		{
			ChangePhase(ScenePhase::Play);
		}
		break;
	case ScenePhase::Play:
		if (Input::GetInstance()->TriggerKey(DIK_SPACE))
		{
			ChangePhase(ScenePhase::End);
		}
		break;
	case ScenePhase::End:
		slide_->Update();
		if (slide_->IsFinish())
		{
			isEnd_ = true;
		}
		break;
	}

	//ゲーム名のオブジェクトの更新
	UpdateGameNameOBJ();
	//ビルの更新
	buildingManager_->Update();
	//天球の更新
	skyDome_->Update(cameraManager_);
	//地面の更新
	ground_->Update(cameraManager_);
	//ゲーム名の更新
	gameName_->Update(cameraManager_);
}

void TitleScene::Draw3D()
{
	//ビルの描画
	buildingManager_->Draw();
	//天球の描画
	skyDome_->Draw();
	//地面の描画
	ground_->Draw();
	//ゲーム名の描画
	gameName_->Draw();
}

void TitleScene::Draw2D()
{
	slide_->Draw();
}

void TitleScene::OnPhaseChanged(ScenePhase newPhase)
{
	switch (newPhase)
	{
	case ScenePhase::Start:
		
		slide_->Start(Slide::Status::SlideOutFromBothSides, 1.0f);
		break;
	case ScenePhase::Play:

		break;
	case ScenePhase::End:
		slide_->Start(Slide::Status::SlideInFromBothSides, 1.0f);
		break;
	}
}

void TitleScene::UpdateGameNameOBJ()
{
	const float deltaTime = 1.0f / 60.0f;
	moveTimer_ += deltaTime;
	rotationTimer_ += deltaTime;

	// 上下に動く処理
	float moveAmplitude = 0.3f; // 移動の振幅
	float moveFrequency = 1.0f; // 移動の周波数 (秒あたりのサイクル数)
	float deltaY = moveAmplitude * std::sin(2.0f * std::numbers::pi_v<float> *moveFrequency * moveTimer_);
	Vector3 newPosition = initialPosition_;
	newPosition.y += deltaY;
	gameName_->SetTranslate(newPosition);

	//1回転する処理
	float rotationSpeed = std::numbers::pi_v<float> / 4.0f;
	rotationAngle_ += rotationSpeed * deltaTime;
	if (rotationAngle_ >= 2.0f * std::numbers::pi_v<float>)
	{
		rotationAngle_ -= 2.0f * std::numbers::pi_v<float>;
	}
	gameName_->SetRotate(Vector3{ 0.0f, rotationAngle_, 0.0f });

	// ゲーム名のオブジェクトの更新
	gameName_->Update(cameraManager_);
}
