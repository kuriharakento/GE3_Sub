#include "GameOverScene.h"

#include "input/Input.h"
#include "SceneManager.h"
#include "2d/SpriteCommon.h"

GameOverScene::~GameOverScene()
{

}

void GameOverScene::Initialize(SceneManager* sceneManager)
{
	sceneManager_ = sceneManager;

	//スライド
	slide_ = std::make_unique<Slide>();
	slide_->Initialize(spriteCommon_);

	//スライドの最初の状態を設定
	slide_->Start(Slide::Status::SlideOutFromBothSides, 1.0f);
	slide_->SetEasingFunc(EaseInOutElastic);
}

void GameOverScene::Update()
{
	switch (currentPhase_)
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
}

void GameOverScene::Draw3D()
{

}

void GameOverScene::Draw2D()
{
	slide_->Draw();
}

void GameOverScene::OnPhaseChanged(ScenePhase newPhase)
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
