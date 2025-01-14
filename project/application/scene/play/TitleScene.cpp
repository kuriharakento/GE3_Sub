#include "TitleScene.h"

#include "audio/Audio.h"
#include "engine/scene/manager/SceneManager.h"
#include "input/Input.h"

void TitleScene::Initialize()
{
	Audio::GetInstance()->LoadWave("fanfare", "Resources/game.wav");
	Audio::GetInstance()->PlayWave("fanfare",true);
}

void TitleScene::Finalize()
{
	
}

void TitleScene::Update()
{
    if (Input::GetInstance()->TriggerKey(DIK_SPACE))
    {
        // 音声を停止
        Audio::GetInstance()->StopWave("fanfare");
    }
    if (Input::GetInstance()->TriggerKey(DIK_UP))
    {
        // 音量を上げる
        Audio::GetInstance()->SetVolume("fanfare", 1.0f); // 最大音量
    }
    if (Input::GetInstance()->TriggerKey(DIK_DOWN))
    {
        // 音量を下げる
        Audio::GetInstance()->SetVolume("fanfare", 0.5f); // 50%の音量
    }
}

void TitleScene::Draw3D()
{

}

void TitleScene::Draw2D()
{

}
