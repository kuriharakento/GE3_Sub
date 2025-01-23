#include "TitleScene.h"

#include "audio/Audio.h"
#include "engine/scene/manager/SceneManager.h"
#include "externals/imgui/imgui.h"
#include "input/Input.h"
#include "manager/TextureManager.h"

void TitleScene::Initialize()
{
	Audio::GetInstance()->LoadWave("fanfare", "game.wav",SoundGroup::BGM);
    // 音声の再生
    Audio::GetInstance()->PlayWave("fanfare", true);

	TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");
	// スプライトの生成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(sceneManager_->GetSpriteCommon(),"./Resources/uvChecker.png");
    sprite_->SetAnchorPoint({ 0.5f,0.5f });
	sprite_->SetSize({ 340.0f,315.0f });
	sprite_->SetPosition({ 200.0f,180.0f });

	// スライドの生成
	slide_ = std::make_unique<Slide>();
	slide_->Initialize(sceneManager_->GetSpriteCommon());

	//デバック用オブジェクトの生成
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(sceneManager_->GetObject3dCommon());
	object3d_->SetModel("sphere.obj");
	object3d_->SetTranslate({ 0.0f,3.0f,1.0f });
}

void TitleScene::Finalize()
{
	
}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("TitleScene");
	#pragma region Debug Sprite
	if (ImGui::CollapsingHeader("Sprite"))
	{
		Vector2 pos = sprite_->GetPosition();
		ImGui::SliderFloat2("Position", &pos.x, 0.0f, 1280.0f);
		sprite_->SetPosition(pos);
		Vector2 size = sprite_->GetSize();
		ImGui::SliderFloat2("Size", &size.x, 0.0f, 1280.0f);
		sprite_->SetSize(size);
		Vector4 color = sprite_->GetColor();
		ImGui::ColorEdit4("Color", &color.x);
		sprite_->SetColor(color);
	}
	#pragma endregion

#pragma region Debug Object3D
	if (ImGui::CollapsingHeader("Object3D"))
	{
		//モデルの変更
		ImGui::Text("Change Model :");
		ImGui::SameLine();
		if (ImGui::Button("cube"))
		{
			object3d_->SetModel("cube.obj");
		}
		ImGui::SameLine();
		if (ImGui::Button("sphere"))
		{
			object3d_->SetModel("sphere.obj");
		}

		Vector3 pos3d = object3d_->GetTranslate();
		ImGui::DragFloat3("Position", &pos3d.x, 0.1f);
		object3d_->SetTranslate(pos3d);
		Vector3 scale = object3d_->GetScale();
		ImGui::DragFloat3("Scale", &scale.x, 0.1f);
		object3d_->SetScale(scale);
		Vector3 rotate = object3d_->GetRotate();
		ImGui::DragFloat3("Rotate", &rotate.x, 0.01f);
		object3d_->SetRotate(rotate);
		Vector4 color3d = object3d_->GetColor();
		ImGui::ColorEdit4("Color", &color3d.x);
		object3d_->SetColor(color3d);
		bool enableLighting = object3d_->IsEnableLighting();
		ImGui::Checkbox("Enable Lighting", &enableLighting);
		object3d_->SetEnableLighting(enableLighting);
		Vector4 lightingColor = object3d_->GetLightingColor();
		ImGui::ColorEdit4("Lighting Color", &lightingColor.x);
		object3d_->SetLightingColor(lightingColor);
		Vector3 lightingDirection = object3d_->GetLightingDirection();
		ImGui::DragFloat3("Lighting Direction", &lightingDirection.x, 0.01f);
		object3d_->SetLightingDirection(lightingDirection);
		float shininess = object3d_->GetShininess();
		ImGui::DragFloat("Shininess", &shininess, 0.1f);
		object3d_->SetShininess(shininess);
	}
#pragma endregion
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

	//オブジェクトの更新
	object3d_->Update(sceneManager_->GetCameraManager());

	// スライドの更新
	slide_->Update();
}

void TitleScene::Draw3D()
{
	object3d_->Draw();
}

void TitleScene::Draw2D()
{
	sprite_->Draw();

	// スライドの描画
	slide_->Draw();
}
