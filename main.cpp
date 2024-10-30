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

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern  IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//自作エンジンに向けたクラス
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

//コードを整理するときに使う
/*--------------[  ]-----------------*/

struct DirectionalLight
{
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct MaterialData
{
	std::string textureFilePath;
};

struct ModelData
{
	std::vector<Sprite::VertexData> vertices;
	MaterialData material;
};


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

///===================================================================
///関数の宣言
///===================================================================


Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes);

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptor, bool shaderVisible);

//DirectXTexを使ってTextureを読むためのLoadTexture関数を作成する
DirectX::ScratchImage LoadTexture(const std::string& filePath);

//DirectX12のTextureResourceを作る
Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);

//TextureResourceにデータを転送する
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

//DepthStencilTextureを作る
Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

//Objファイルを読む関数
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

//Materialファイルを読み込む
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

///===================================================================
///グローバル変数の宣言
///===================================================================

TextureManager* TextureManager::instance_ = nullptr;


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	D3DResourceLeakChecker leakCheck;

	///===================================================================
	///
	///ポインタ置き場
	///
	///===================================================================

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

	

	



	///===================================================================
	///モデルの生成読み込み
	///===================================================================

	////モデル読み込み
	//ModelData modelData = LoadObjFile("Resources", "axis.obj");
	////頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(VertexData) * modelData.vertices.size());
	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();				//リソースの先頭のアドレスから使う
	//vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());	//使用するリソースのサイズは頂点のサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);		//１頂点当たりのサイズ

	//VertexData* vertexData = nullptr;
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());


	///===================================================================
	///Resourceにデータを書き込む
	///===================================================================

	

	////左下
	//vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	//vertexData[0].texcoord = { 0.0f,1.0f };
	////上
	//vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	//vertexData[1].texcoord = { 0.5f,0.0f };
	////右下
	//vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	//vertexData[2].texcoord = { 1.0f,1.0f };


	////左下２
	//vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	//vertexData[3].texcoord = { 0.0f,1.0f };
	////上２
	//vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
	//vertexData[4].texcoord = { 0.5f,0.0f };
	////右下２
	//vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	//vertexData[5].texcoord = { 1.0f,1.0f };


	///===================================================================
	///平行光源用のリソース
	///===================================================================

	DirectionalLight* directionalLightData = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(DirectionalLight));

	directionalLightResource->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(&directionalLightData)
	);

	//デフォルト値は以下のようにしておく
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = Normalize({ 0.0f,-1.0f,0.0f });
	directionalLightData->intensity = 1.0f;


	///===================================================================
	///Material用のResourceを作る
	///===================================================================

	////マテリアル用のリソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(Material));

	////マテリアルにデータを書き込む
	//Material* materialData = nullptr;

	////書き込むためのアドレスを取得
	//materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	////色を変える
	//materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	////Lightingを有効にする
	//materialData->enableLighting = true;

	//materialData->uvTransform = MakeIdentity4x4();

	///===================================================================
	///TransformationMatrix用のResourceを作る
	///===================================================================

	////WVP用のリソースを作る。Matrix4x4　１つ分サイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(TransformationMatrix));
	////データを書き込む
	//TransformationMatrix* wvpData = nullptr;
	////書き込むためのアドレスを取得
	//wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	////単位行列を書き込んでおく

	//wvpData->WVP = MakeIdentity4x4();
	//wvpData->World = MakeIdentity4x4();


	////Transform変数を作る
	//Transform transform{
	//	{1.0f,1.0f,1.0f},
	//	{0.0f,0.0f,0.0f},
	//	{0.0f,0.0f,0.0f}
	//};


	//Transform cameraTransform{
	//	{1.0f,1.0f,1.0f},
	//	{0.0f,0.0f,0.0f},
	//	{0.0f,0.0,-10.0f}
	//};

	////UVTransform用の変数
	//Transform uvTransformSprite{
	//	{ 1.0f,1.0f,1.0f },
	//	{ 0.0f,0.0f,0.0f },
	//	{ 0.0f,0.0f,0.0f }
	//};

	//Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	//Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	//Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	//Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	//Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	//wvpData->WVP = worldViewProjectionMatrix;
	//wvpData->World = worldMatrix;

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



	///===================================================================
	///Textureを読んで転送する
	///===================================================================

	//1枚目
	DirectX::ScratchImage mipImages = LoadTexture("Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(dxCommon->GetDevice(), metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource, mipImages, dxCommon->GetDevice(), dxCommon->GetCommandList());

	////2枚目
	//DirectX::ScratchImage mipImages2 = LoadTexture(modelData.material.textureFilePath);
	//const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = CreateTextureResource(dxCommon->GetDevice(), metadata2);
	//Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource2 = UploadTextureData(textureResource2, mipImages2, dxCommon->GetDevice(), dxCommon->GetCommandList());

	///===================================================================
	///実際にShaderResourceViewを作る
	///===================================================================

	//metadataをもとにSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	////metadata2をもとにSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	//srvDesc2.Format = metadata2.format;
	//srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//SRVを制作するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	//先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVの生成
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	//SRVを制作するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = dxCommon->GetSRVCPUDescriptorHandle(2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = dxCommon->GetSRVGPUDescriptorHandle(2);

	//SRVの生成
	/*dxCommon->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);*/

	///===================================================================
	///シーンの初期化
	///===================================================================

	//入力の初期化
	Input* input = new Input();
	input->Initialize(winApp);


	

	///===================================================================
	///変数
	///===================================================================

	bool useMonsterBall = true;
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

	///===================================================================
	///
	///メインループ
	///
	///===================================================================

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

		///===================================================================
		///
		///ゲームの処理		更新処理
		///
		///===================================================================


		//===================================================
		//入力
		//===================================================

		//入力の更新
		input->Update();

		

		//3Dモデルの更新
		////vertexDataの変換
		//worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		//cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		//viewMatrix = Inverse(cameraMatrix);
		//projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
		//worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		//wvpData->WVP = worldViewProjectionMatrix;
		//wvpData->World = worldMatrix;

		//スプライトの更新
		sprite->Update();

		for(int i = 0; i < spriteCount; ++i)
		{
			sprites[i]->Update();
		}


		//===================================================
		//ImGui
		//===================================================
#ifdef _DEBUG
		ImGui::Begin("Setting");
		ImGui::End();
		/*--------------[ スプライト ]-----------------*/

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

		

#endif
		//ゲームの処理が終わり描画処理に入る前にImGuiの内部コマンドを生成する
		ImGui::Render();

		///===================================================================
		///
		///ゲームの処理		描画処理
		///
		///===================================================================



		//描画前処理
		dxCommon->PreDraw();

		//スプライトの描画準備。共通の設定を行う
		spriteCommon->CommonRenderingSetting();

		sprite->Draw();

		for(int i = 0; i < spriteCount; ++i)
		{
			sprites[i]->Draw();
		}


		//3D描画
		//dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
		////形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
		//dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		////マテリアルCBufferの場所を設定
		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		////wvp用のCBufferの場所を設定
		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
		////SRVのDescriptorTableの先頭を設定。2はrootPatameter[2]である。
		//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

		//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
		////描画！
		//dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);


		//実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
		
		//描画後処理
		dxCommon->PostDraw();
	}

	//ImGuiの終了処理。
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//解放処理
	winApp->Finalize();
	delete winApp;
	TextureManager::GetInstance()->Finalize();
	delete dxCommon;
	delete spriteCommon;
	delete input;
	delete sprite;


#ifdef _DEBUG

#endif


	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}

	return 0;
}



Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource = nullptr;
	//頂点リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//頂点リソースの設定
	D3D12_RESOURCE_DESC bufferResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferResourceDesc.Width = sizeInBytes;
	//バッファの場合はこれらは１にする決まり
	bufferResourceDesc.Height = 1;
	bufferResourceDesc.DepthOrArraySize = 1;
	bufferResourceDesc.MipLevels = 1;
	bufferResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(hr));

	return bufferResource;
}


DirectX::ScratchImage LoadTexture(const std::string& filePath)
{
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(
		filePathW.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB,
		nullptr,
		image
	);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB,
		0,
		mipImages
	);
	assert(SUCCEEDED(hr));

	//mipmap付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata)
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);									//Textureの幅
	resourceDesc.Height = UINT(metadata.height);								//Textureの高さ
	resourceDesc.MipLevels = UINT(metadata.mipLevels);							//mipmapの数
	resourceDesc.DepthOrArraySize = UINT(metadata.arraySize);					//奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;										//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;											//サンプリングカウント。１固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);		//Textureの次元数。普段使っているのは２次元

	//利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;								//細かい設定を行う

	//resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));

	return resource;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERI_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height)
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;											//Textureの幅
	resourceDesc.Height = height;										//Textureの高さ
	resourceDesc.MipLevels = 1;											//mipmapの数
	resourceDesc.DepthOrArraySize = 1;									//奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;									//サンプリングカウント。１個指定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;		//２次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;		//DepthStencilとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;						//VRAM上に作る

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;							//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				//フォーマット。Resourceと合わせる

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,												//Heapの設定
		D3D12_HEAP_FLAG_NONE,											//Heapの特殊な設定。特になし
		&resourceDesc,													//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,								//深度値を書き込む状態にしておく
		&depthClearValue,												//Clear最適値
		IID_PPV_ARGS(&resource)											//作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	return resource;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;				//構築するModelData
	std::vector<Vector4> positions;		//位置
	std::vector<Vector3> normals;		//法線
	std::vector<Vector2> texcoords;		//テクスチャ座標
	std::string line;					//ファイルから読んだ１行を格納するもの

	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	//先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		} else if (identifier == "f")
		{
			Sprite::VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
			{
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して。頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];

				position.x *= -1.0f;
				normal.x *= -1.0f;

				texcoord.y = 1.0f - texcoord.y;

				/*VertexData vertex = { position,texcoord,normal };
				modelData.vertices.push_back(vertex);*/
				triangle[faceVertex] = { position,texcoord,normal };
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);;
		} else if (identifier == "mtllib")
		{
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので,ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;

			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	return materialData;
}