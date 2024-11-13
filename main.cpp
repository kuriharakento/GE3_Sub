#include <Windows.h>
#include <cstdint>
#include <string>
#include<cassert>
#include <DirectXMath.h>
#include <format>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <wrl.h>
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"

//DirectX12
#include<d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include<dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")

//DXC
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

//ImGui
#include "Object3d.h"
#include "Object3dCommon.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern  IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////
///					>>>自作クラスのインクルード<<<						///
///////////////////////////////////////////////////////////////////////

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "StringUtility.h"
#include "TextureManager.h"

//数学
#include "VectorFunc.h"
#include "MatrixFunc.h"

//描画に使うデータ型
#include "GraphicsTypes.h"

//コードを整理するときに使う
/*--------------[  ]-----------------*/

///////////////////////////////////////////////////////////////////////
///							>>>構造体の宣言<<<						///
///////////////////////////////////////////////////////////////////////

struct D3DResourceLeakChecker
{
	~D3DResourceLeakChecker()
	{
		//リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

class ResourceObject
{
public:
	ResourceObject(ID3D12Resource* resource)
		:resource_(resource)
	{}
	~ResourceObject()
	{
		if (resource_)
		{
			resource_->Release();
		}
	}
	ID3D12Resource* Get() { return resource_; }

private:
	ID3D12Resource* resource_;
};

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

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);

	//スプライト共通部の初期化
	SpriteCommon* spriteCommon = new SpriteCommon();
	spriteCommon->Initialize(dxCommon);

	//3Dオブジェクト共通部の初期化
	Object3dCommon* objectCommon = new Object3dCommon();
	objectCommon->Initialize(dxCommon);

	//モデル共通部の初期化
	ModelCommon* modelCommon = new ModelCommon();
	modelCommon->Initialize(dxCommon);
#pragma region 球体
	///===================================================================
	///頂点位置を計算する
	///===================================================================

	////分割数
	//const uint32_t kSubdivision = 16;
	////π(円周率)
	//const float pi = std::numbers::pi_v<float>;
	////経度分割１つ分の角度φ
	//const float kLonEvery = pi * 2.0f / float(kSubdivision);
	////緯度分割１つ分の角度Θ
	//const float kLatEvery = pi / float(kSubdivision);
	////緯度の方向に分割
	//for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
	//{
	//	//緯度
	//	float lat = -pi / 2.0f + kLatEvery * latIndex;		//Θ
	//	//texcoord

	//	//経度の方向に分割しながら線を描く
	//	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
	//	{
	//		uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
	//		//texcoord
	//		float v = 1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision);
	//		float u = static_cast<float>(lonIndex) / static_cast<float>(kSubdivision);
	//		//経度
	//		float lon = lonIndex * kLonEvery;	//φ


	//		///===================================================================
	//		///1枚目の三角形
	//		///===================================================================

	//		//頂点にデータを入力する。基準点a 左下
	//		vertexData[start].position.x = std::cos(lat) * std::cos(lon);
	//		vertexData[start].position.y = std::sin(lat);
	//		vertexData[start].position.z = std::cos(lat) * std::sin(lon);
	//		vertexData[start].position.w = 1.0f;
	//		vertexData[start].texcoord = { u,v };
	//		vertexData[start].normal.x = vertexData[start].position.x;
	//		vertexData[start].normal.y = vertexData[start].position.y;
	//		vertexData[start].normal.z = vertexData[start].position.z;

	//		//残りの５頂点も順番に計算して入力していく
	//		//基準点b　左上
	//		start++;
	//		vertexData[start].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
	//		vertexData[start].position.y = std::sin(lat + kLatEvery);
	//		vertexData[start].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
	//		vertexData[start].position.w = 1.0f;
	//		vertexData[start].texcoord = { u,v - 1.0f / kSubdivision };
	//		vertexData[start].normal.x = vertexData[start].position.x;
	//		vertexData[start].normal.y = vertexData[start].position.y;
	//		vertexData[start].normal.z = vertexData[start].position.z;

	//		//基準点c　右下
	//		start++;
	//		vertexData[start].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
	//		vertexData[start].position.y = std::sin(lat);
	//		vertexData[start].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
	//		vertexData[start].position.w = 1.0f;
	//		vertexData[start].texcoord = { u + 1.0f / kSubdivision,v };
	//		vertexData[start].normal.x = vertexData[start].position.x;
	//		vertexData[start].normal.y = vertexData[start].position.y;
	//		vertexData[start].normal.z = vertexData[start].position.z;


	//		///===================================================================
	//		///２枚目の三角形
	//		///===================================================================

	//		//基準点b　左上
	//		start++;
	//		vertexData[start].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
	//		vertexData[start].position.y = std::sin(lat + kLatEvery);
	//		vertexData[start].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
	//		vertexData[start].position.w = 1.0f;
	//		vertexData[start].texcoord = { u,v - 1.0f / kSubdivision };
	//		vertexData[start].normal.x = vertexData[start].position.x;
	//		vertexData[start].normal.y = vertexData[start].position.y;
	//		vertexData[start].normal.z = vertexData[start].position.z;

	//		//基準点d　右上
	//		start++;
	//		vertexData[start].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
	//		vertexData[start].position.y = std::sin(lat + kLatEvery);
	//		vertexData[start].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
	//		vertexData[start].position.w = 1.0f;
	//		vertexData[start].texcoord = { u + 1.0f / kSubdivision ,v - 1.0f / kSubdivision };
	//		vertexData[start].normal.x = vertexData[start].position.x;
	//		vertexData[start].normal.y = vertexData[start].position.y;
	//		vertexData[start].normal.z = vertexData[start].position.z;

	//		//基準点c 右下
	//		start++;
	//		vertexData[start].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
	//		vertexData[start].position.y = std::sin(lat);
	//		vertexData[start].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
	//		vertexData[start].position.w = 1.0f;
	//		vertexData[start].texcoord = { u + 1.0f / kSubdivision,v };
	//		vertexData[start].normal.x = vertexData[start].position.x;
	//		vertexData[start].normal.y = vertexData[start].position.y;
	//		vertexData[start].normal.z = vertexData[start].position.z;
	//	}
	//}
#pragma endregion

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
	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteCommon,"./Resources/monsterBall.png");

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

	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelCommon);

	//3Dオブジェクトの初期化
	std::unique_ptr<Object3d> object = std::make_unique<Object3d>();
	object->Initialize(objectCommon);
	object->SetModel(model.get());


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

	
	////////////////////////////////////////////////////////////
	///					>>>オブジェクトの解放<<<				///
	////////////////////////////////////////////////////////////

	/*--------------[ スプライトの解放 ]-----------------*/

	//単体
	delete sprite;

	//複数
	for(int i = 0; i < spriteCount; ++i)
	{
		sprites[i].reset();
	}

	/*--------------[ オブジェクトの解放 ]-----------------*/


	////////////////////////////////////////////////////////////
	///					>>>オブジェクトの開放<<<				///
	////////////////////////////////////////////////////////////


	///////////////////////////////////
	///		>>>基盤システム<<<		///
	///////////////////////////////////

	//NOTE:ここは基本的に触らない

	winApp->Finalize();								//ウィンドウアプリケーションの終了処理
	delete winApp;									//ウィンドウアプリケーションの解放
	TextureManager::GetInstance()->Finalize();		//テクスチャマネージャーの終了処理
	delete dxCommon;								//DirectXCommonの解放
	delete spriteCommon;							//スプライト共通部の解放
	delete modelCommon;								//モデル共通部の解放
	delete objectCommon;							//3Dオブジェクト共通部の解放
	delete input;									//入力の解放


	///////////////////////////////////////////////////////////////////////
	///						>>>解放処理ここまで<<<							///
	///////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}

	return 0;
#endif
}