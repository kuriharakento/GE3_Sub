#include "TitleScene.h"
#include "SceneManager.h"
#include "input/Input.h"
#include "2d/SpriteCommon.h"

void TitleScene::Initialize(SceneManager* sceneManager)
{
	sceneManager_ = sceneManager;

	slide_ = std::make_unique<Slide>();
	slide_->Initialize(spriteCommon_);

	//スライド
	slide_->Start(Slide::Status::SlideOutFromBothSides, 1.25f);
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
			sceneManager_->ChangeScene("GameScene");
		}
		break;
	}
}

void TitleScene::Draw3D()
{

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
		
		slide_->Start(Slide::Status::SlideOutFromBothSides, 1.25f);
		break;
	case ScenePhase::Play:

		break;
	case ScenePhase::End:
		slide_->Start(Slide::Status::SlideInFromBothSides, 1.25f);
		break;
	}
}
