#include "TitleScene.h"

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
#include "engine/effects/ParticleManager.h"
#include "manager/TextureManager.h"

void TitleScene::Initialize()
{
	Audio::GetInstance()->LoadWave("fanfare", "game.wav", SoundGroup::BGM);
	// 音声の再生
	Audio::GetInstance()->PlayWave("fanfare", true);

	// スプライトの生成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(sceneManager_->GetSpriteCommon(), "Resources/uvChecker.png");
	sprite_->SetSize({ 150.0f,150.0f });
	sprite_->SetPosition({ 0.0f,0.0f });

	//デバック用オブジェクトの生成
	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(sceneManager_->GetObject3dCommon());
	object3d_->SetModel("highPolygonSphere.obj");
	object3d_->SetTranslate({ 0.0f,0.0f,1.0f });
	object3d_->SetDirectionalLightIntensity(0.0f);
	object3d_->SetLightManager(sceneManager_->GetLightManager());

	//地面の生成
	terrain_ = std::make_unique<Object3d>();
	terrain_->Initialize(sceneManager_->GetObject3dCommon());
	terrain_->SetModel("terrain.obj");
	terrain_->SetTranslate({ 0.0f,0.0f,1.0f });
	terrain_->SetDirectionalLightIntensity(0.0f);
	terrain_->SetLightManager(sceneManager_->GetLightManager());

	//平面オブジェクトの生成
	plane_ = std::make_unique<Object3d>();
	plane_->Initialize(sceneManager_->GetObject3dCommon());
	plane_->SetModel("plane.gltf");
	plane_->SetTranslate({ -1.0f,1.0f,1.0f });

	//パーティクルグループの作成


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
	splineCamera_->LoadJson("spline.json");
	splineCamera_->Start(0.001f, true);
	splineCamera_->SetTarget(&object3d_->GetTranslate());

	//エミッターの初期化
	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize("test", "./Resources/gradationLine.png");
	//emitter_->Initialize("test", "./Resources/uvChecker.png");
	emitter_->SetEmitRange({ -2.0f,-2.0f,-2.0f }, { 2.0f, 2.0f, 2.0f });
	emitter_->Start(
		{ 2.0f,2.0f,2.0f },
		3,
		10.0f,
		true
	);
	emitter_->SetEmitRate(0.2f);
	//emitter_->SetModelType(ParticleGroup::ParticleType::Plane);
	emitter_->SetModelType(ParticleGroup::ParticleType::Cylinder);
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

	static bool splineCameraUpdate = true;
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

	//Jsonエディタの表示
	JsonEditorManager::GetInstance()->RenderEditUI();

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
		//plane
		Vector3 pos3dPlane = plane_->GetTranslate();
		ImGui::DragFloat3("PositionPlane", &pos3dPlane.x, 0.1f);
		plane_->SetTranslate(pos3dPlane);
		Vector3 scalePlane = plane_->GetScale();
		ImGui::DragFloat3("ScalePlane", &scalePlane.x, 0.1f);
		plane_->SetScale(scalePlane);
		Vector3 rotatePlane = plane_->GetRotate();
		ImGui::DragFloat3("RotatePlane", &rotatePlane.x, 0.01f);
		plane_->SetRotate(rotatePlane);
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

	//地面の更新
	terrain_->Update(sceneManager_->GetCameraManager());

	//平面オブジェクトの更新
	plane_->Update(sceneManager_->GetCameraManager());

}

void TitleScene::Draw3D()
{
	//3Dオブジェクトの描画
	object3d_->Draw();

	//地面の描画
	terrain_->Draw();

	//平面オブジェクトの描画
	plane_->Draw();
  
	// ラインの描画
	splineCamera_->DrawSplineLine();
}

void TitleScene::Draw2D()
{
	// スプライトの描画
	sprite_->Draw();
}
