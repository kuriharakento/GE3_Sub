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
	//音声リソースの読み込み
	Audio::GetInstance()->LoadWave("fanfare", "game.wav", SoundGroup::BGM);
	// 音声の再生
	Audio::GetInstance()->PlayWave("fanfare", true);
	//エネミーマネージャーの初期化
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(sceneManager_->GetObject3dCommon(), sceneManager_->GetCameraManager(), sceneManager_->GetLightManager(), "cube.obj");
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("TitleScene");
	ImGui::Text("LineCount: %d", sceneManager_->GetLineManager()->GetLineCount());
	ImGui::End();
#endif
	//エネミーマネージャーの更新
	enemyManager_->Update();
}

void TitleScene::Draw3D()
{
	//ラインの描画
	sceneManager_->GetLineManager()->DrawGrid(100.0f, 1.0f, VectorColorCodes::White);
	//エネミーマネージャーの描画
	enemyManager_->Draw();
	sceneManager_->GetLineManager()->DrawSphere(enemyManager_->GetTargetPosition(), 1.0f, VectorColorCodes::White);
}

void TitleScene::Draw2D()
{

}
