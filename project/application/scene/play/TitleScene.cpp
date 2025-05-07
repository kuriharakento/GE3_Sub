#include "TitleScene.h"

#include "audio/Audio.h"
#include "engine/scene/manager/SceneManager.h"
#include "externals/imgui/imgui.h"
#include "input/Input.h"
#include "jsonEditor/JsonEditorManager.h"
#include "lighting/VectorColorCodes.h"
#include "line/LineManager.h"
#include "manager/ParticleManager.h"
#include "manager/TextureManager.h"
#include "application/GameObject/collider/ColliderAABB.h"

void TitleScene::Initialize()
{
	Audio::GetInstance()->LoadWave("fanfare", "game.wav", SoundGroup::BGM);
	// 音声の再生
	Audio::GetInstance()->PlayWave("fanfare", true);

	//デバック用オブジェクトの生成
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(sceneManager_->GetObject3dCommon());
	object3d_->SetModel("cube.obj");
	object3d_->SetTranslate({ 0.0f,0.0f,1.0f });
	object3d_->SetDirectionalLightIntensity(0.0f);
	object3d_->SetLightManager(sceneManager_->GetLightManager());

	//スカイドームの生成
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(sceneManager_->GetObject3dCommon(), "skydome.obj");

	//パーティクルグループの作成
	ParticleManager::GetInstance()->CreateParticleGroup("plane", "./Resources/gradationLine.png");
	ParticleManager::GetInstance()->CreateParticleGroup("Ring", "./Resources/uvChecker.png");

	//Jsonエディタ
	JsonEditorManager::GetInstance()->Initialize();

	//オービットカメラワークの生成
	orbitCameraWork_ = std::make_unique<OrbitCameraWork>();
	orbitCameraWork_->Initialize(sceneManager_->GetCameraManager()->GetActiveCamera());
	orbitCameraWork_->SetPositionOffset({ 0.0f,2.0f,0.0f });
	orbitCameraWork_->Start(
		&object3d_->GetTranslate(),
		10.0f,
		1.0f
	);

	//スプラインカメラの生成
	splineCamera_ = std::make_unique<SplineCamera>();
	splineCamera_->Initialize(sceneManager_->GetCameraManager()->GetActiveCamera());
	splineCamera_->SetlineManager(sceneManager_->GetLineManager());
	splineCamera_->LoadJson("spline.json");
	splineCamera_->Start(0.001f, true);
	splineCamera_->SetTarget(&object3d_->GetTranslate());

	//ゲームオブジェクトの生成
	player = std::make_unique<GameObject>();
	player->Initialize(sceneManager_->GetObject3dCommon(),sceneManager_->GetCameraManager()->GetActiveCamera());
	//衝突コンポーネントの追加
	/*auto playerCollision = std::make_shared<CollisionComponent>(player.get(),std::make_shared<ColliderAABB>());
	playerCollision->SetDefaultCallbacks();
	player->AddComponent("Collision", playerCollision);*/

	enemy = std::make_unique<GameObject>();
	enemy->Initialize(sceneManager_->GetObject3dCommon(), sceneManager_->GetCameraManager()->GetActiveCamera());
	//衝突コンポーネントの追加
	/*auto enemyCollision = std::make_shared<CollisionComponent>(enemy.get(), std::make_shared<ColliderAABB>());
	enemyCollision->SetDefaultCallbacks();
	enemy->AddComponent("Collision", enemyCollision);*/

}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("TitleScene");

	static bool splineCameraUpdate = false;
	static bool orbitCameraUpdate = false;

	//カメラワークの更新
	ImGui::Checkbox("orbitCamera Update", &orbitCameraUpdate);
	ImGui::Checkbox("splineCamera Update", &splineCameraUpdate);
	// カメラワークの更新
	if (orbitCameraUpdate)
	{
		orbitCameraWork_->Update();
	}

	if (splineCameraUpdate)
	{
		splineCamera_->Update();
	}

	if(ImGui::CollapsingHeader("line"))
	{
		ImGui::DragFloat3("CubePos1", &cubePos1_.x, 0.1f);
		ImGui::DragFloat3("CubePos2", &cubePos2_.x, 0.1f);
		ImGui::DragFloat3("SpherePos1", &spherePos1_.x, 0.1f);
		ImGui::DragFloat3("SpherePos2", &spherePos2_.x, 0.1f);
	}

	//Jsonエディタの表示
	JsonEditorManager::GetInstance()->RenderEditUI();

	player->Update();

	enemy->Update();

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
			object3d_->SetModel("highPolygonSphere.obj");
		}
	#pragma region Transform
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
	#pragma endregion

	#pragma region Lighting
		bool enableLighting = object3d_->IsEnableLighting();
		ImGui::Checkbox("Enable Lighting", &enableLighting);
		object3d_->SetEnableLighting(enableLighting);
		//ディレクショナルライトの設定
		#pragma region DirectionalLight
		if (ImGui::CollapsingHeader("DirectionalLight")) {
			Vector4 lightingColor = object3d_->GetLightingColor();
			ImGui::ColorEdit4("Lighting Color", &lightingColor.x);
			object3d_->SetLightingColor(lightingColor);
			Vector3 lightingDirection = object3d_->GetLightingDirection();
			ImGui::DragFloat3("Lighting Direction", &lightingDirection.x, 0.01f,-1.0f,1.0f);
			object3d_->SetLightingDirection(lightingDirection);
			float shininess = object3d_->GetShininess();
			ImGui::DragFloat("Shininess", &shininess, 0.1f);
			object3d_->SetShininess(shininess);
		}
		#pragma endregion
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

	//オブジェクトの更新
	object3d_->Update(sceneManager_->GetCameraManager());

	//スカイドームの更新
	skydome_->Update(sceneManager_->GetCameraManager());
}

void TitleScene::Draw3D()
{
	//3Dオブジェクトの描画
	object3d_->Draw();

	player->Draw(sceneManager_->GetCameraManager()->GetActiveCamera());

	enemy->Draw(sceneManager_->GetCameraManager()->GetActiveCamera());
  
	//スカイドームの描画
	skydome_->Draw();
  
	// ラインの描画
	sceneManager_->GetLineManager()->DrawGrid(300.0f, 5.0f, VectorColorCodes::White);
	splineCamera_->DrawSplineLine();
}

void TitleScene::Draw2D()
{

}
