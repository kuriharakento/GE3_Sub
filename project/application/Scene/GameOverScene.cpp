#include "GameOverScene.h"

#include "input/Input.h"
#include "SceneManager.h"

void GameOverScene::Initialize(SceneManager* sceneManager)
{
	sceneManager_ = sceneManager;
}

void GameOverScene::Update()
{
	switch (currentPhase_)
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
		sceneManager_->ChangeScene("TitleScene");
		break;
	}
}

void GameOverScene::Draw3D()
{

}

void GameOverScene::Draw2D()
{

}

void GameOverScene::OnPhaseChanged(ScenePhase newPhase)
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
