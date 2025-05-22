#include "PostProcessManager.h"
#include "PostProcessManager.h"
#include "engine/base/DirectXCommon.h"
#include "engine/manager/SrvManager.h"
#include "DirectXTex/d3dx12.h"
#include <cassert>

PostProcessManager::PostProcessManager() {}

PostProcessManager::~PostProcessManager() {}

void PostProcessManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	grayscaleEffect_ = std::make_unique<GrayscaleEffect>();
	grayscaleEffect_->Initialize(dxCommon, srvManager, L"Resources/shaders/Grayscale.VS.hlsl", L"Resources/shaders/Grayscale.PS.hlsl");

	fullScreenEffect_ = std::make_unique<FullScreenEffect>();
	fullScreenEffect_->Initialize(dxCommon, srvManager, L"Resources/shaders/FullScreen.VS.hlsl", L"Resources/shaders/FullScreen.PS.hlsl");

	// 中間リソースの作成
	CreateIntermediateResources();
}

void PostProcessManager::CreateIntermediateResources()
{
	// 画面サイズの中間テクスチャ2枚を作成
	auto device = dxCommon_->GetDevice();
	UINT width = WinApp::kClientWidth;
	UINT height = WinApp::kClientHeight;

	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	texDesc.SampleDesc.Count = 1;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;

	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// tempTextureA
	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET, // ←ここを修正
		&clearValue,
		IID_PPV_ARGS(&tempTextureA_)
	);
	// tempTextureB
	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET, // ←ここを修正
		&clearValue,
		IID_PPV_ARGS(&tempTextureB_)
	);

	// SRV/RTVの作成（SrvManager等でSRV/RTVを作成し、ハンドルを取得）
	// ここでは仮にSrvManagerでSRV/RTVを作成し、インデックスを取得する例
	uint32_t srvIndexA = srvManager_->Allocate();
	uint32_t srvIndexB = srvManager_->Allocate();
	srvManager_->CreateSRVforTexture2D(srvIndexA, tempTextureA_.Get(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);
	srvManager_->CreateSRVforTexture2D(srvIndexB, tempTextureB_.Get(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);
	tempSRVA_ = srvManager_->GetGPUDescriptorHandle(srvIndexA);
	tempSRVB_ = srvManager_->GetGPUDescriptorHandle(srvIndexB);

	// RTVはDirectXCommonで作成したヒープを使う
	auto rtvHeap = dxCommon_->GetRTVDescriptorHeap();
	UINT rtvSize = dxCommon_->GetDescriptorSizeRTV();
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// 2,3番をPing-Pong用に使う
	tempRTVA_ = dxCommon_->GetCPUDescriptorHandle(rtvHeap, rtvSize, 2);
	tempRTVB_ = dxCommon_->GetCPUDescriptorHandle(rtvHeap, rtvSize, 3);

	device->CreateRenderTargetView(tempTextureA_.Get(), &rtvDesc, tempRTVA_);
	device->CreateRenderTargetView(tempTextureB_.Get(), &rtvDesc, tempRTVB_);

}

void PostProcessManager::Draw(D3D12_GPU_DESCRIPTOR_HANDLE inputTexture)
{
	auto* cmdList = dxCommon_->GetCommandList();

	// tempTextureA_ をRTVとして使う前
	if (tempTextureAState_ == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	{
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = tempTextureA_.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cmdList->ResourceBarrier(1, &barrier);
		tempTextureAState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	// グレースケールエフェクトを tempTextureA_ に描画
	grayscaleEffect_->UpdateParameters();
	grayscaleEffect_->Draw(inputTexture, tempRTVA_);

	// tempTextureA_ をSRVとして使う前
	if (tempTextureAState_ == D3D12_RESOURCE_STATE_RENDER_TARGET)
	{
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = tempTextureA_.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cmdList->ResourceBarrier(1, &barrier);
		tempTextureAState_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	}

	// スワップチェインのRTVに描画
	UINT backBufferIndex = dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE swapchainRTV = dxCommon_->GetRTVHandle(backBufferIndex);
	cmdList->OMSetRenderTargets(1, &swapchainRTV, FALSE, nullptr);

	fullScreenEffect_->Draw(tempSRVA_, swapchainRTV);
}
