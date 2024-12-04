#include <Windows.h>
#include <vector>

//ImGui
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif

///////////////////////////////////////////////////////////////////////
///					>>>自作クラスのインクルード<<<						///
///////////////////////////////////////////////////////////////////////

#pragma region 自作クラスのインクルード
#include "input/Input.h"
#include "base/WinApp.h"
#include "base/DirectXCommon.h"
#include "base/D3DResourceLeakChecker.h"
#include "2d/SpriteCommon.h"
#include "2d/Sprite.h"
#include "manager/TextureManager.h"
#include "3d/Object3dCommon.h"
#include "3d/Object3d.h"
#include "3d/ModelManager.h"
#include "manager/CameraManager.h"
#include "manager/ImGuiManager.h"
#include "manager/SrvManager.h"
#include "math/VectorFunc.h"
#pragma endregion

//コードを整理するときに使う
/*--------------[  ]-----------------*/

///////////////////////////////////////////////////////////////////////
///							>>>構造体の宣言<<<						///
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
///						>>>関数の宣言<<<								///
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
///						>>>グローバル変数の宣言<<<						///
///////////////////////////////////////////////////////////////////////

//テクスチャマネージャーのインスタンス
TextureManager* TextureManager::instance_ = nullptr;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	D3DResourceLeakChecker leakCheck;


	///////////////////////////////////////////////////////////////////////
	///						>>>基盤システム初期化<<<						///
	///////////////////////////////////////////////////////////////////////

	//ウィンドウアプリケーションの初期化
	WinApp* winApp = new WinApp();
	winApp->Initialize();

	//DirectXCoomonの初期化
	DirectXCommon* dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//SRVマネージャーの初期化
	std::unique_ptr<SrvManager> srvManager = std::make_unique<SrvManager>();
	srvManager->Initialize(dxCommon);

	//ImGuiの初期化
	ImGuiManager* imguiManager = new ImGuiManager();
	imguiManager->Initilize(winApp,dxCommon,srvManager.get());

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxCommon,srvManager.get());

	//スプライト共通部の初期化
	SpriteCommon* spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	//3Dオブジェクト共通部の初期化
	Object3dCommon* objectCommon = new Object3dCommon();
	objectCommon->Initialize(dxCommon);

	//3Dモデルマネージャーの初期化
	ModelManager::GetInstance()->Initialize(dxCommon);

	///////////////////////////////////////////////////////////////////////
	///						>>>汎用機能初期化<<<							///
	///////////////////////////////////////////////////////////////////////

	//入力の初期化
	Input* input = new Input();
	input->Initialize(winApp);

	//カメラの初期化
	/*std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	camera->SetRotate({});
	camera->SetTranslate({ 0.0f,4.0f,-10.0f });
	objectCommon->SetDefaultCamera(camera.get());*/

	//カメラマネージャーの初期化
	std::unique_ptr<CameraManager> cameraManager = std::make_unique<CameraManager>();
	cameraManager->AddCamera("main");
	cameraManager->SetActiveCamera("main");
	cameraManager->GetActiveCamera()->SetTranslate({ 0.0f,4.0f,-10.0f });
	cameraManager->GetActiveCamera()->SetRotate({ 0.0f,0.0f,0.0f });

	objectCommon->SetDefaultCamera(cameraManager->GetActiveCamera());

	///////////////////////////////////////////////////////////////////////
	///						>>>変数の宣言<<<								///
	///////////////////////////////////////////////////////////////////////

	/*--------------[ スプライト ]-----------------*/

	#pragma region テクスチャの読み込み
	//テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture("./Resources/monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");

	#pragma endregion

	#pragma region 宣言と初期化
	//スプライトの初期化
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
	sprite->Initialize(spriteCommon,"./Resources/uvChecker.png");

	//スプライト（複数）
	std::vector<std::unique_ptr<Sprite>> sprites;
	int spriteCount = 5;
	for(int i = 0; i < spriteCount; ++i)
	{
		std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize(spriteCommon,"./Resources/uvChecker.png");
		sprite->SetSize({ 100.0f,100.0f });
		sprite->SetPosition({ 200.0f * i,0.0f });
		sprites.push_back(std::move(sprite));
	}
	#pragma endregion

	/*--------------[ 3Dオブジェクト ]-----------------*/

	#pragma region モデルの読み込み
	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	#pragma endregion

	#pragma region 宣言と初期化
	//3Dオブジェクトの初期化
	std::unique_ptr<Object3d> object = std::make_unique<Object3d>();
	object->Initialize(objectCommon);
	object->SetModel("plane.obj");
	object->SetTranslate({ 0.0f,3.0f,0.0f });

	std::unique_ptr<Object3d> objectAxis = std::make_unique<Object3d>();
	objectAxis->Initialize(objectCommon);
	objectAxis->SetModel("axis.obj");
	objectAxis->SetTranslate({ 5.0f,3.0f,0.0f });

	#pragma endregion

	///////////////////////////////////////////////////////////////////////
	///																	///
	///						>>>メインループ<<<							///
	///																	///
	///////////////////////////////////////////////////////////////////////

	//ゲームループ
	while (true)
	{
		//windowsノメッセージ処理
		if (winApp->ProcessMessage())
		{
			//ゲームループを抜ける
			break;
		}

		//フレームの先頭でImGuiに、ここからフレームが始まる旨を告げる
		imguiManager->Begin();

		///////////////////////////////////////////////////////////////////////
		///						>>>更新処理ここから<<<							///
		///////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////
		///		>>>汎用機能の更新ここから<<<		///
		///////////////////////////////////////////
		
		//入力の更新
		input->Update();

		//カメラの更新
		cameraManager->Update();

		///////////////////////////////////////////
		///		>>>汎用機能の更新ここまで<<<		///
		///////////////////////////////////////////

		//スプライトの更新
		sprite->Update();

		//3Dオブジェクトの更新
		object->Update(cameraManager.get());
		objectAxis->Update(cameraManager.get());

		//スプライト（複数）
		for(int i = 0; i < spriteCount; ++i)
		{
			sprites[i]->Update();
		}

		
#ifdef _DEBUG
		/*--------------[ ImGui ]-----------------*/

		#pragma region シーン全体の設定
		ImGui::Begin("Setting");

		ImGui::End();
		#pragma endregion

		#pragma region
		ImGui::Begin("CameraManager");
		if(ImGui::Button("Set Camera"))
		{
			object->SetCamera(cameraManager->GetActiveCamera());
			objectAxis->SetCamera(cameraManager->GetActiveCamera());
		}
		ImGui::End();
		#pragma endregion

		#pragma region スプライト用のImGui
		ImGui::Begin("Sprite");
		//スプライトの位置
		Vector2 position = sprite->GetPosition();
		ImGui::DragFloat2("translate",&position.x,1.0f,-1280.0f,1280.0f,"%.1f");
		sprite->SetPosition(position);
		//スプライトの回転
		float rotation = sprite->GetRotation();
		ImGui::SliderAngle("rotate", &rotation, 0.0f, 360.0f);
		sprite->SetRotation(rotation);
		//スプライトの色
		Vector4 color = sprite->GetColor();
		ImGui::ColorEdit4("color", &color.x);
		sprite->SetColor(color);
		//スプライトのサイズ
		Vector2 size = sprite->GetSize();
		ImGui::DragFloat2("size", &size.x, 1.0f);
		sprite->SetSize(size);
		//スプライトのアンカーポイント
		Vector2 anchorPoint = sprite->GetAnchorPoint();
		ImGui::DragFloat2("anchorPoint", &anchorPoint.x, 0.01f, 0.0f, 1.0f);
		sprite->SetAnchorPoint(anchorPoint);
		//スプライトの切り出しサイズ
		Vector2 drawSize = sprite->GetTextureSize();
		ImGui::DragFloat2("drawSize", &drawSize.x, 1.0f);
		sprite->SetTextureSize(drawSize);
		//反転
		ImGui::Text("Flip");
		ImGui::SameLine();
		if(ImGui::Button("FlipX"))
		{
			bool flipX = !sprite->GetFlipX();
			sprite->SetFlipX(flipX);
		}
		ImGui::SameLine();
		if(ImGui::Button("FlipY"))
		{
			bool flipY = !sprite->GetFlipY();
			sprite->SetFlipY(flipY);
		}
		//スプライトのテクスチャ
		ImGui::Text("Change Texture");
		ImGui::SameLine();
		if(ImGui::Button("MonsterBall"))
		{
			sprite->SetTexture("./Resources/monsterBall.png");
		}
		ImGui::SameLine();
		if(ImGui::Button("UVChecker"))
		{
			sprite->SetTexture("./Resources/uvChecker.png");
		}
		ImGui::End();
		#pragma endregion

		#pragma region 3Dオブジェクト用のImGui
		ImGui::Begin("Object3D");
		//3Dオブジェクトの位置
		Vector3 objectPosition = object->GetTranslate();
		ImGui::DragFloat3("translate", &objectPosition.x, 0.1f, -10.0f, 10.0f);
		object->SetTranslate(objectPosition);
		//3Dオブジェクトの回転
		Vector3 objectRotate = object->GetRotate();
		ImGui::DragFloat3("rotate", &objectRotate.x, 0.01f,-3.14f,3.14f);
		object->SetRotate(objectRotate);
		//3Dオブジェクトの拡大縮小
		Vector3 objectScale = object->GetScale();
		ImGui::DragFloat3("scale", &objectScale.x, 0.1f, 0.0f, 10.0f);
		object->SetScale(objectScale);
		ImGui::End();

		ImGui::Begin("Object3D Axis");
		//座標
		Vector3 objectAxisPosition = objectAxis->GetTranslate();
		ImGui::DragFloat3("translate", &objectAxisPosition.x, 0.1f, -10.0f, 10.0f);
		objectAxis->SetTranslate(objectAxisPosition);
		//回転
		Vector3 objectAxisRotate = objectAxis->GetRotate();
		ImGui::DragFloat3("rotate", &objectAxisRotate.x, 0.01f, -3.14f, 3.14f);
		objectAxis->SetRotate(objectAxisRotate);
		//拡大縮小
		Vector3 objectAxisScale = objectAxis->GetScale();
		ImGui::DragFloat3("scale", &objectAxisScale.x, 0.1f, 0.0f, 10.0f);
		objectAxis->SetScale(objectAxisScale);
		ImGui::End();

		#pragma endregion

#endif

		///////////////////////////////////////////////////////////////////////
		///						>>>更新処理ここまで<<<							///
		///////////////////////////////////////////////////////////////////////

		//ゲームの処理が終わり描画処理に入る前にImGuiの内部コマンドを生成する
		imguiManager->End();

		///////////////////////////////////////////////////////////////////////
		///						>>>描画処理ここから<<<							///
		///////////////////////////////////////////////////////////////////////

		//描画前処理
		dxCommon->PreDraw();
		srvManager->PreDraw();
		

		/*--------------[ 3Dオブジェクトの描画 ]-----------------*/

		//NOTE:3Dオブジェクトの描画準備。共通の設定を行う
		objectCommon->CommonRenderingSetting();

		object->Draw();
		objectAxis->Draw();

		/*--------------[ スプライトの描画 ]-----------------*/

		//スプライトの描画準備。共通の設定を行う
		spriteCommon->CommonRenderingSetting();

		sprite->Draw();

		for(int i = 0; i < spriteCount; ++i)
		{
		//	sprites[i]->Draw();
		}

		//実際のcommandListのImGuiの描画コマンドを積む
		//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());

		imguiManager->Draw();

		//描画後処理
		dxCommon->PostDraw();

		///////////////////////////////////////////////////////////////////////
		///						>>>描画処理ここまで<<<							///
		///////////////////////////////////////////////////////////////////////
	}

	///////////////////////////////////////////////////////////////////////
	///						>>>解放処理ここから<<<							///
	///////////////////////////////////////////////////////////////////////
		
	/*--------------[ スプライトの解放 ]-----------------*/



	/*--------------[ オブジェクトの解放 ]-----------------*/



	///////////////////////////////////
	///		>>>基盤システム<<<		///
	///////////////////////////////////

	//NOTE:ここは基本的に触らない
	winApp->Finalize();								//ウィンドウアプリケーションの終了処理
	delete winApp;									//ウィンドウアプリケーションの解放
	imguiManager->Finalize();						//ImGuiManagerの終了処理
	delete imguiManager;							//ImGuiManagerの解放
	TextureManager::GetInstance()->Finalize();		//テクスチャマネージャーの終了処理
	delete dxCommon;								//DirectXCommonの解放
	delete spriteCommon;							//スプライト共通部の解放
	delete objectCommon;							//3Dオブジェクト共通部の解放
	ModelManager::GetInstance()->Finalize();		//3Dモデルマネージャーの終了処理
	delete input;									//入力の解放


	return 0;
}