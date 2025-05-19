#include "TitleScene.h"

#include "application/GameObject/component/action/FireComponent.h"
#include "audio/Audio.h"
#include "base/PostProcessPass.h"
#include "effects/component/single/AccelerationComponent.h"
#include "effects/component/single/ColorFadeOutComponent.h"
#include "effects/component/single/DragComponent.h"
#include "effects/component/single/GravityComponent.h"
#include "effects/component/group/MaterialColorComponent.h"
#include "effects/component/single/OrbitComponent.h"
#include "effects/component/single/RandomInitialVelocityComponent.h"
#include "effects/component/single/RotationComponent.h"
#include "effects/component/single/ScaleOverLifetimeComponent.h"
#include "effects/component/group/UVRotateComponent.h"
#include "effects/component/group/UVScaleComponent.h"
#include "effects/component/group/UVTranslateComponent.h"
#include "engine/scene/manager/SceneManager.h"
#include "externals/imgui/imgui.h"
#include "input/Input.h"
#include "jsonEditor/JsonEditorManager.h"
#include "lighting/VectorColorCodes.h"
#include "line/LineManager.h"
#include "application/GameObject/component/collision/AABBColliderComponent.h"
#include "application/GameObject/component/action/MoveComponent.h"
#include "application/GameObject/component/collision/CollisionManager.h"
#include "engine/effects/ParticleManager.h"
#include "manager/TextureManager.h"

void TitleScene::Initialize()
{
	Audio::GetInstance()->LoadWave("fanfare", "game.wav", SoundGroup::BGM);
	// 音声の再生
	Audio::GetInstance()->PlayWave("fanfare", true);

	//地面の生成
	terrain_ = std::make_unique<Object3d>();
	terrain_->Initialize(sceneManager_->GetObject3dCommon());
	terrain_->SetModel("terrain.obj");
	terrain_->SetTranslate({ 0.0f,0.0f,1.0f });
	terrain_->SetDirectionalLightIntensity(0.0f);
	terrain_->SetLightManager(sceneManager_->GetLightManager());	//パーティクルグループの作成

	//Jsonエディタ
	JsonEditorManager::GetInstance()->Initialize();

	//当たり判定マネージャーの初期化
	CollisionManager::GetInstance()->Initialize();

	//ゲームオブジェクトの生成
	player = std::make_unique<Player>("player");
	player->Initialize(sceneManager_->GetObject3dCommon(), sceneManager_->GetLightManager());
	player->AddComponent("MoveComponent", std::make_shared<MoveComponent>(5.0f)); // 移動速度
	player->AddComponent("FireComponent", std::make_shared<FireComponent>(sceneManager_->GetObject3dCommon(), sceneManager_->GetLightManager()));
	//衝突判定コンポーネント
	player->AddComponent("AABBCollider", std::make_shared<OBBColliderComponent>(player.get()));


	enemy = std::make_unique<GameObject>("enemy");
	enemy->Initialize(sceneManager_->GetObject3dCommon(), sceneManager_->GetLightManager(), sceneManager_->GetCameraManager()->GetActiveCamera());
	enemy->SetPosition({ 0.0f,1.0f,10.0f });
	//衝突判定コンポーネント
	enemy->AddComponent("AABBCollider", std::make_shared<AABBColliderComponent>(enemy.get()));


	//オービットカメラワークの生成
	orbitCameraWork_ = std::make_unique<OrbitCameraWork>();
	orbitCameraWork_->Initialize(sceneManager_->GetCameraManager()->GetActiveCamera());
	orbitCameraWork_->SetPositionOffset({ 0.0f,2.0f,0.0f });
	orbitCameraWork_->Start(
		&player->GetPosition(),
		10.0f,
		1.0f
	);

	//スプラインカメラの生成
	splineCamera_ = std::make_unique<SplineCamera>();
	splineCamera_->Initialize(sceneManager_->GetCameraManager()->GetActiveCamera());
	splineCamera_->LoadJson("spline.json");
	splineCamera_->Start(0.001f, true);
	splineCamera_->SetTarget(&player->GetPosition());

	//フォローカメラの生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(sceneManager_->GetCameraManager()->GetActiveCamera());
	followCamera_->Start(
		&player->GetPosition(),
		15.0f,
		0.06f
	);

	//トップダウンカメラの生成
	topDownCamera_ = std::make_unique<TopDownCamera>();
	topDownCamera_->Initialize(sceneManager_->GetCameraManager()->GetActiveCamera());
	topDownCamera_->SetOffset({ 0.0f, 0.0f, -4.0f });
	topDownCamera_->SetPitch(1.1f);
	topDownCamera_->Start(
		70.0f,
		&player->GetPosition()
	);

	//エミッターの初期化
	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize("test", "./Resources/gradationLine.png");
	//emitter_->SetTexture("Resources/uvChecker.png");
	emitter_->SetEmitRange({ -2.0f,-2.0f,-2.0f }, { 2.0f, 2.0f, 2.0f });
	emitter_->Start(
		&player->GetPosition(),
		3,
		10.0f,
		true
	);
	emitter_->SetEmitRate(0.2f);
	//emitter_->SetModelType(ParticleGroup::ParticleType::Plane);
	//emitter_->SetModelType(ParticleGroup::ParticleType::Cylinder);
	//emitter_->SetModelType(ParticleGroup::ParticleType::Sphere);
	//emitter_->SetModelType(ParticleGroup::ParticleType::Torus);
	//emitter_->SetModelType(ParticleGroup::ParticleType::Star);
	//emitter_->SetModelType(ParticleGroup::ParticleType::Heart);
	emitter_->SetModelType(ParticleGroup::ParticleType::Spiral);
	//emitter_->SetModelType(ParticleGroup::ParticleType::Cone);
	//emitter_->SetBillborad(true);
	emitter_->SetBillborad(false);
	//======コンポーネントの追加=========================
	emitter_->AddComponent(std::make_shared<GravityComponent>(Vector3{ 0.0f, 0.2f, 0.0f }));
	// 空気抵抗コンポーネントを追加
	emitter_->AddComponent(std::make_shared<DragComponent>(0.98f));

	// スケール変化コンポーネントを追加
	emitter_->AddComponent(std::make_shared<ScaleOverLifetimeComponent>(1.0f, 0.0f));

	// 色フェードアウトコンポーネントを追加
	emitter_->AddComponent(std::make_shared<ColorFadeOutComponent>());

	// 初期速度ランダム化コンポーネントを追加
	emitter_->AddComponent(std::make_shared<RandomInitialVelocityComponent>(
		Vector3{ -1.0f, 2.0f, -1.0f }, Vector3{ 1.0f, 5.0f, 1.0f }));
	// 回転コンポーネントを追加
	emitter_->AddComponent(std::make_shared<RotationComponent>(Vector3{ 0.0f, 0.1f, 0.0f }));
	// 軌道コンポーネントを追加 (中心座標、半径、速度)
	emitter_->AddComponent(std::make_shared<OrbitComponent>(
		Vector3{ 0.0f, 0.0f, 0.0f }, 5.0f, 0.05f));
	// 加速度コンポーネントを追加
	emitter_->AddComponent(std::make_shared<AccelerationComponent>(Vector3{ 0.0f, 0.01f, 0.0f }));

	// UV変換コンポーネント
	emitter_->AddComponent(std::make_shared<UVTranslateComponent>(Vector3{ 0.5f, 0.0f, 0.0f })); // UVをX方向に毎フレーム0.01移動
	//emitter_->AddComponent(std::make_shared<UVRotateComponent>(Vector3{ 0.0f, 0.0f, 0.01f }));    // UVをZ軸周りに毎フレーム0.01ラジアン回転
	//emitter_->AddComponent(std::make_shared<UVScaleComponent>(Vector3{ 0.005f, 0.005f, 0.0f }));   // UVを毎フレーム1.005倍に拡大

	// マテリアル色変更コンポーネント
	emitter_->AddComponent(std::make_shared<MaterialColorComponent>(VectorColorCodes::Cyan));

}

void TitleScene::Finalize()
{
	CollisionManager::GetInstance()->Finalize();
}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("TitleScene");
	Vector3 uvtranslate = emitter_->GetUVTranslate();
	ImGui::DragFloat3("UVTranslate", &uvtranslate.x, 0.01f);
	emitter_->SetUVTranslate(uvtranslate);
	Vector3 uvscale = emitter_->GetUVScale();
	ImGui::DragFloat3("UVScale", &uvscale.x, 0.01f);
	emitter_->SetUVScale(uvscale);
	Vector3 uvrotate = emitter_->GetUVRotate();
	ImGui::DragFloat3("UVRotate", &uvrotate.x, 0.01f);
	emitter_->SetUVRotate(uvrotate);

	static bool isGrayScale = false;
	if (ImGui::Checkbox("GrayScale", &isGrayScale))
	{
		sceneManager_->GetPostProcessPass()->SetGrayscale(isGrayScale);
	}

	static bool splineCameraUpdate = false;
	static bool orbitCameraUpdate = false;
	static bool followCameraUpdate = false;
	static bool topDownCameraUpdate = false;

	//カメラワークの更新
	ImGui::Checkbox("orbitCamera Update", &orbitCameraUpdate);
	ImGui::Checkbox("splineCamera Update", &splineCameraUpdate);
	ImGui::Checkbox("followCamera Update", &followCameraUpdate);
	ImGui::Checkbox("topDownCamera Update", &topDownCameraUpdate);
	if (Input::GetInstance()->TriggerKey(DIK_F))
	{
		followCameraUpdate = !followCameraUpdate;
	}
	// カメラワークの更新
	if (orbitCameraUpdate)
	{
		orbitCameraWork_->Update();
	}
	if (splineCameraUpdate)
	{
		splineCamera_->Update();
	}
	if (followCameraUpdate)
	{
		followCamera_->Update();
	}
	if (topDownCameraUpdate)
	{
		topDownCamera_->Update();
	}

	//Jsonエディタの表示
	JsonEditorManager::GetInstance()->RenderEditUI();

#pragma region GameObject
	if (ImGui::CollapsingHeader("GameObject"))
	{
		ImGui::Text("Player");
		Vector3 playerPos = player->GetPosition();
		ImGui::DragFloat3("Player Position", &playerPos.x, 0.1f);
		player->SetPosition(playerPos);
		ImGui::Text("Enemy");
		Vector3 enemyPos = enemy->GetPosition();
		ImGui::DragFloat3("Enemy Position", &enemyPos.x, 0.1f);
		enemy->SetPosition(enemyPos);
	}
#pragma endregion


#pragma region Particle
	if (ImGui::CollapsingHeader("Particle"))
	{
		
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

	// 地面の更新
	terrain_->Update(sceneManager_->GetCameraManager());

	//キャラクターの更新
	player->Update();
	enemy->Update();

	//衝突判定開始
	CollisionManager::GetInstance()->CheckCollisions();

}

void TitleScene::Draw3D()
{
	player->Draw(sceneManager_->GetCameraManager());

	enemy->Draw(sceneManager_->GetCameraManager());

	//スカイドームの描画
	terrain_->Draw();

	// グリッドの描画
	LineManager::GetInstance()->DrawGrid(
		300.0f,
		5.0f,
		VectorColorCodes::White
	);

	splineCamera_->DrawSplineLine();
}

void TitleScene::Draw2D()
{

}
