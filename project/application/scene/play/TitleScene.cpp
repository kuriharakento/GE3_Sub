#include "TitleScene.h"

#include "audio/Audio.h"
#include "engine/scene/manager/SceneManager.h"
#include "externals/imgui/imgui.h"
#include "input/Input.h"
#include "manager/TextureManager.h"

void TitleScene::Initialize()
{
	Audio::GetInstance()->LoadWave("fanfare", "fanfare.wav",SoundGroup::BGM);
    // 音声の再生
    Audio::GetInstance()->PlayWave("fanfare", true);

	TextureManager::GetInstance()->LoadTexture("./Resources/monsterBall.png");

	// スプライトの生成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(sceneManager_->GetSpriteCommon(),"./Resources/monsterBall.png");
    sprite_->SetAnchorPoint({ 0.5f,0.5f });
}

void TitleScene::Finalize()
{
	
}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("TitleScene");
	Vector2 pos = sprite_->GetPosition();
	ImGui::SliderFloat2("Position", &pos.x, 0.0f, 1280.0f);
	sprite_->SetPosition(pos);
	ImGui::End();
#endif
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
	if (Input::GetInstance()->TriggerKey(DIK_LEFT))
	{
		// フェードイン
		Audio::GetInstance()->FadeIn("fanfare", 2.0f); // 2秒かけてフェードイン
	}
	if (Input::GetInstance()->TriggerKey(DIK_RIGHT))
	{
		// フェードアウト
		Audio::GetInstance()->FadeOut("fanfare", 2.0f); // 2秒かけてフェードアウト
	}

	// スプライトの更新
	sprite_->Update();

}

void TitleScene::Draw3D()
{

}

void TitleScene::Draw2D()
{
	sprite_->Draw();
}
