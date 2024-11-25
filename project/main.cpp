#include <Windows.h>
#include <vector>

//ImGui
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern  IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
#include "base/TextureManager.h"
#include "3d/Object3dCommon.h"
#include "3d/Object3d.h"
#include "3d/ModelManager.h"
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
	//リークチェック
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

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);

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

	///////////////////////////////////////////////////////////////////////
	///						>>>変数の宣言<<<								///
	///////////////////////////////////////////////////////////////////////

	//テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture("./Resources/monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("./Resources/uvChecker.png");


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

	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("plane.obj");

	//3Dオブジェクトの初期化
	std::unique_ptr<Object3d> object = std::make_unique<Object3d>();
	object->Initialize(objectCommon);
	object->SetModel("plane.obj");
	object->SetTranslate({ 0.0f,3.0f,0.0f });

	std::unique_ptr<Object3d> objectAxis = std::make_unique<Object3d>();

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
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		///////////////////////////////////////////////////////////////////////
		///						>>>更新処理ここから<<<							///
		///////////////////////////////////////////////////////////////////////

		//入力の更新
		input->Update();

		//スプライトの更新
		sprite->Update();

		//3Dオブジェクトの更新
		object->Update();

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

		#pragma region スプライト用のImGui
		ImGui::Begin("Sprite");
		//スプライトの位置
		Vector2 position = sprite->GetPosition();
		ImGui::DragFloat2("translate",&position.x,1.0f,-1280.0f,1280.0f);
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
		#pragma endregion

#endif

		//ゲームの処理が終わり描画処理に入る前にImGuiの内部コマンドを生成する
		ImGui::Render();

		///////////////////////////////////////////////////////////////////////
		///						>>>更新処理ここまで<<<							///
		///////////////////////////////////////////////////////////////////////



		///////////////////////////////////////////////////////////////////////
		///						>>>描画処理ここから<<<							///
		///////////////////////////////////////////////////////////////////////

		//描画前処理
		dxCommon->PreDraw();

		/*--------------[ 3Dオブジェクトの描画 ]-----------------*/

		//NOTE:3Dオブジェクトの描画準備。共通の設定を行う
		objectCommon->CommonRenderingSetting();

		object->Draw();

		/*--------------[ スプライトの描画 ]-----------------*/

		//スプライトの描画準備。共通の設定を行う
		spriteCommon->CommonRenderingSetting();

		sprite->Draw();

		for(int i = 0; i < spriteCount; ++i)
		{
		//	sprites[i]->Draw();
		}

		//実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
		
		//描画後処理
		dxCommon->PostDraw();

		///////////////////////////////////////////////////////////////////////
		///						>>>描画処理ここまで<<<							///
		///////////////////////////////////////////////////////////////////////
	}

	///////////////////////////////////////////////////////////////////////
	///						>>>解放処理ここから<<<							///
	///////////////////////////////////////////////////////////////////////

	 ///////////////////////////////////
	 ///	>>>ImGuiの終了処理<<<		///
	 ///////////////////////////////////

	//NOTE:ここは基本的に触らない

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	
	///////////////////////////////////////
	///		>>>ゲームオブジェクト<<<		///
	///////////////////////////////////////
	
	/*--------------[ スプライトの解放 ]-----------------*/



	/*--------------[ オブジェクトの解放 ]-----------------*/



	///////////////////////////////////
	///		>>>基盤システム<<<		///
	///////////////////////////////////

	//NOTE:ここは基本的に触らない
	winApp->Finalize();								//ウィンドウアプリケーションの終了処理
	delete winApp;									//ウィンドウアプリケーションの解放
	TextureManager::GetInstance()->Finalize();		//テクスチャマネージャーの終了処理
	delete dxCommon;								//DirectXCommonの解放
	delete spriteCommon;							//スプライト共通部の解放
	delete objectCommon;							//3Dオブジェクト共通部の解放
	ModelManager::GetInstance()->Finalize();		//3Dモデルマネージャーの終了処理
	delete input;									//入力の解放


	return 0;
}