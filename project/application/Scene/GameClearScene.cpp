#include "GameClearScene.h"

#include "input/Input.h"
#include "SceneManager.h"

GameClearScene::~GameClearScene()
{

}

void GameClearScene::Initialize(SceneManager* sceneManager)
{
	sceneManager_ = sceneManager;
}

void GameClearScene::Update()
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

void GameClearScene::Draw3D()
{

}

void GameClearScene::Draw2D()
{

}

void GameClearScene::OnPhaseChanged(ScenePhase newPhase)
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
