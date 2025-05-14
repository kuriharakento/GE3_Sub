#include "TitleScene.h"

#include "application/GameObject/component/action/FireComponent.h"
#include "audio/Audio.h"
#include "base/PostProcessPass.h"
#include "engine/scene/manager/SceneManager.h"
#include "externals/imgui/imgui.h"
#include "input/Input.h"
#include "jsonEditor/JsonEditorManager.h"
#include "lighting/VectorColorCodes.h"
#include "line/LineManager.h"
#include "manager/ParticleManager.h"
#include "application/GameObject/component/collision/AABBColliderComponent.h"
#include "application/GameObject/component/action/MoveComponent.h"
#include "application/GameObject/component/collision/CollisionManager.h"

void TitleScene::Initialize()
{
	Audio::GetInstance()->LoadWave("fanfare", "game.wav", SoundGroup::BGM);
	// 音声の再生
	Audio::GetInstance()->PlayWave("fanfare", true);

	//スカイドームの生成
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(sceneManager_->GetObject3dCommon(), "skydome.obj");

	//パーティクルグループの作成
	ParticleManager::GetInstance()->CreateParticleGroup("plane", "./Resources/gradationLine.png");
	ParticleManager::GetInstance()->CreateParticleGroup("Ring", "./Resources/uvChecker.png");

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
}

void TitleScene::Finalize()
{
	CollisionManager::GetInstance()->Finalize();
}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("TitleScene");
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
		static Vector3 pos = {};
		ImGui::DragFloat3("Emit pos", &pos.x);
		//ビルボードの有効無効
		if (ImGui::Button("Billboard On"))
		{
			ParticleManager::GetInstance()->SetBillboard("test", true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Billboard Off"))
		{
			ParticleManager::GetInstance()->SetBillboard("test", false);
		}
		//生成
		if (ImGui::Button("Emit"))
		{
			ParticleManager::GetInstance()->Emit("test", pos, 100);
		}
		ImGui::SameLine();
		if (ImGui::Button("Emit Ring"))
		{
			ParticleManager::GetInstance()->EmitRing("test", pos,5);
			ParticleManager::GetInstance()->SetRandomRotate("test");
			//ParticleManager::GetInstance()->SetRandomScale("test");
		}
		ImGui::SameLine();
		if (ImGui::Button("Emit Plane"))
		{
			ParticleManager::GetInstance()->EmitPlane("test", pos, 100);
			ParticleManager::GetInstance()->SetRandomRotate("test");
			//ParticleManager::GetInstance()->SetRandomScale("test");
		}
		ImGui::SameLine();
		if (ImGui::Button("Emit Cylinder"))
		{
			ParticleManager::GetInstance()->EmitCylinder("test", pos, 1);
			//ParticleManager::GetInstance()->SetRandomRotate("test");
		}
		//テクスチャの変更
		if (ImGui::Button("Change Texture: uvChecker"))
		{
			ParticleManager::GetInstance()->SetTexture("test", "./Resources/uvChecker.png");
		}
		if (ImGui::Button("Change Texture: black"))
		{
			ParticleManager::GetInstance()->SetTexture("test", "./Resources/black.png");
		}
		//頂点データの変更
		if (ImGui::Button("Change VertexData: Plane"))
		{
			ParticleManager::GetInstance()->SetVertexData("test", VertexShape::Plane);
		}
		if (ImGui::Button("Change VertexData: Ring"))
		{
			ParticleManager::GetInstance()->SetVertexData("test", VertexShape::Ring);
		}
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

	//スカイドームの更新
	skydome_->Update(sceneManager_->GetCameraManager());

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
	skydome_->Draw();
  
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
