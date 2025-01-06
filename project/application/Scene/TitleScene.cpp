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
	slide_->Start(Slide::Status::SlideOutFromBothSides, 1.0f);
}

void TitleScene::Update()
{
	switch(currentPhase_)
	{
	case ScenePhase::Start:
		
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
		if (slide_->IsFinish())
		{
			isEnd_ = true;
		}
		break;
	}
	slide_->Update();
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
		
		slide_->Start(Slide::Status::SlideOutFromBothSides, 1.0f);
		break;
	case ScenePhase::Play:

		break;
	case ScenePhase::End:
		slide_->Start(Slide::Status::SlideInFromBothSides, 1.0f);
		break;
	}
}
