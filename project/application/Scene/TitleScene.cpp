#include "TitleScene.h"
#include "SceneManager.h"
#include "input/Input.h"

void TitleScene::Initialize(SceneManager* sceneManager)
{
	sceneManager_ = sceneManager;
}

void TitleScene::Update()
{
	switch(currentPhase_)
	{
	case ScenePhase::Start:
		ChangePhase(ScenePhase::Play);
		break;
	case ScenePhase::Play:
		if (Input::GetInstance()->TriggerKey(DIK_SPACE))
		{
			ChangePhase(ScenePhase::End);
		}
		break;
	case ScenePhase::End:
		sceneManager_->ChangeScene("GameScene");
		break;
	}
}

void TitleScene::Draw3D()
{

}

void TitleScene::Draw2D()
{
}

void TitleScene::OnPhaseChanged(ScenePhase newPhase)
{
	switch (newPhase)
	{
	case ScenePhase::Start:

		break;
	case ScenePhase::Play:

		break;
	case ScenePhase::End:

		break;
	}
}
