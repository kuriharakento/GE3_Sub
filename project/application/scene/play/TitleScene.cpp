#include "TitleScene.h"

#include "audio/Audio.h"
#include "engine/scene/manager/SceneManager.h"
#include "input/Input.h"

void TitleScene::Initialize()
{
	Audio::GetInstance()->LoadWave("fanfare", "Resources/fanfare.wav");
	Audio::GetInstance()->PlayWave("fanfare",true);
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
