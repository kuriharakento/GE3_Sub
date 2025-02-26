#include "TitleScene.h"

#include "audio/Audio.h"
#include "engine/scene/manager/SceneManager.h"
#include "externals/imgui/imgui.h"
#include "input/Input.h"
#include "lighting/VectorColorCodes.h"
#include "line/LineManager.h"
#include "manager/ParticleManager.h"
#include "manager/TextureManager.h"

void TitleScene::Initialize()
{
	//カメラの設定
	sceneManager_->GetCameraManager()->GetActiveCamera()->SetTranslate({ 0.0f,20.0f,-20.0f });
	sceneManager_->GetCameraManager()->GetActiveCamera()->SetRotate({ 0.6f,0.0f,0.0f });

	Audio::GetInstance()->LoadWave("fanfare", "game.wav", SoundGroup::BGM);
	// 音声の再生
	Audio::GetInstance()->PlayWave("fanfare", true);

	//タックルエネミーの初期化
	tackleEnemy_ = std::make_unique<TackleEnemy>();
	tackleEnemy_->Initialize(sceneManager_->GetObject3dCommon(), "cube.obj");
	tackleEnemy_->SetLightManager(sceneManager_->GetLightManager());
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("TitleScene");
	ImGui::End();
#endif
	//タックルエネミーの更新
	tackleEnemy_->Update(sceneManager_->GetCameraManager());

}

void TitleScene::Draw3D()
{
	sceneManager_->GetLineManager()->DrawGrid(100.0f, 1.0f, VectorColorCodes::White);
	//タックルエネミーの描画
	tackleEnemy_->Draw();
	sceneManager_->GetLineManager()->DrawSphere(tackleEnemy_->GetTargetPosition(), 0.5f, VectorColorCodes::Red);
}

void TitleScene::Draw2D()
{

}
