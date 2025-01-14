#include "GamePlayScene.h"
#include "input/Input.h"
#include "application/scene/manager/SceneManager.h"


void GamePlayScene::Initialize()
{

}

void GamePlayScene::Finalize()
{

}

void GamePlayScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("TITLE");
	}
}

void GamePlayScene::Draw3D()
{

}

void GamePlayScene::Draw2D()
{

}
