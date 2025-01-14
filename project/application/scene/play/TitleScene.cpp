#include "TitleScene.h"

#include "application/scene/manager/SceneManager.h"
#include "input/Input.h"

void TitleScene::Initialize()
{

}

void TitleScene::Finalize()
{

}

void TitleScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GAMEPLAY");
	}
}

void TitleScene::Draw3D()
{

}

void TitleScene::Draw2D()
{

}
