#include "GameOverScene.h"

#include "input/Input.h"
#include "scene/manager/SceneManager.h"

void GameOverScene::Initialize()
{
	//スライド
	slide_ = std::make_unique<Slide>();
	slide_->Initialize(sceneManager_->GetSpriteCommon());

	//スライドの最初の状態を設定
	slide_->Start(Slide::Status::SlideOutFromBothSides, 1.0f);
	slide_->SetEasingFunc(EaseInOutElastic);

	//ゲームオーバースプライト
	gameOver_ = std::make_unique<Sprite>();
	gameOver_->Initialize(sceneManager_->GetSpriteCommon(), "./Resources/gameOver.png");
}

void GameOverScene::Finalize()
{

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
			sceneManager_->ChangeScene("TITLE");
		}
		break;
	}

	gameOver_->Update();
}

void GameOverScene::Draw3D()
{

}

void GameOverScene::Draw2D()
{
	gameOver_->Draw();

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
