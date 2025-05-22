#include "PostProcessManager.h"
#include "engine/base/DirectXCommon.h"
#include "engine/manager/SrvManager.h"
#include "DirectXTex/d3dx12.h"
#include <cassert>

#include "ImGui/imgui.h"

PostProcessManager::PostProcessManager() {}

PostProcessManager::~PostProcessManager() {}

void PostProcessManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	auto vignetteEffect_ = std::make_unique<VignetteEffect>();
	vignetteEffect_->Initialize(dxCommon, srvManager, this);
	postEffects_.push_back(std::move(vignetteEffect_));

	auto grayscaleEffect_ = std::make_unique<GrayscaleEffect>();
	grayscaleEffect_->Initialize(dxCommon, srvManager, this);
	postEffects_.push_back(std::move(grayscaleEffect_));	

	//フルスクリーンエフェクト
	fullScreenEffect_ = std::make_unique<FullScreenEffect>();
	fullScreenEffect_->Initialize(dxCommon, srvManager, L"Resources/shaders/FullScreen.VS.hlsl", L"Resources/shaders/FullScreen.PS.hlsl");

	CreateComComputePipelineState(L"Resources/shaders/PostProcessCompute.hlsl");

	// 中間リソースの作成
	CreateIntermediateResources();
}

void PostProcessManager::CreateIntermediateResources()
{
	auto device = dxCommon_->GetDevice();
	UINT width = WinApp::kClientWidth;
	UINT height = WinApp::kClientHeight;

	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// UAVとRTV両方許可
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

	// テクスチャ作成（初期状態をUAVに設定）
	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		&clearValue,
		IID_PPV_ARGS(&tempTextureA_)
	);
	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		&clearValue,
		IID_PPV_ARGS(&tempTextureB_)
	);

	// 状態を初期化
	tempTextureAState_ = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	tempTextureBState_ = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	// SRV/UAV作成（srvManager_で作成・管理）
	uint32_t srvIndexA = srvManager_->Allocate();
	uint32_t srvIndexB = srvManager_->Allocate();
	uint32_t uavIndexA = srvManager_->Allocate();
	uint32_t uavIndexB = srvManager_->Allocate();

	srvManager_->CreateSRVforTexture2D(srvIndexA, tempTextureA_.Get(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);
	srvManager_->CreateSRVforTexture2D(srvIndexB, tempTextureB_.Get(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

	// UAVディスクリプタ作成
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	uavDesc.Texture2D.PlaneSlice = 0;

	auto srvHeap = srvManager_->GetSrvHeap();
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CPU_DESCRIPTOR_HANDLE uavHandleA = srvHeap->GetCPUDescriptorHandleForHeapStart();
	uavHandleA.ptr += uavIndexA * descriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE uavHandleB = srvHeap->GetCPUDescriptorHandleForHeapStart();
	uavHandleB.ptr += uavIndexB * descriptorSize;

	device->CreateUnorderedAccessView(tempTextureA_.Get(), nullptr, &uavDesc, uavHandleA);
	device->CreateUnorderedAccessView(tempTextureB_.Get(), nullptr, &uavDesc, uavHandleB);

	// GPUディスクリプタハンドルも取得
	tempSRVA_ = srvManager_->GetGPUDescriptorHandle(srvIndexA);
	tempSRVB_ = srvManager_->GetGPUDescriptorHandle(srvIndexB);
	tempUAVA_ = srvManager_->GetGPUDescriptorHandle(uavIndexA);
	tempUAVB_ = srvManager_->GetGPUDescriptorHandle(uavIndexB);
}

void PostProcessManager::CreateComComputePipelineState(const std::wstring& csPath)
{
	// ルートパラメータ（SRVとUAVとCBVを想定）
	CD3DX12_DESCRIPTOR_RANGE srvRange{};
	srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE uavRange{};
	uavRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	CD3DX12_ROOT_PARAMETER rootParams[4]{};
	// ルートパラメータ0: 入力テクスチャ (SRV, t0)
	rootParams[0].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_ALL);
	// ルートパラメータ1: 出力テクスチャ (UAV, u0)
	rootParams[1].InitAsDescriptorTable(1, &uavRange, D3D12_SHADER_VISIBILITY_ALL);
	// ルートパラメータ2: Vignetteエフェクトの定数バッファ (CBV, b0)
	// register(b0) に対応します
	rootParams[2].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	// ルートパラメータ3: Grayscaleエフェクトの定数バッファ (CBV, b1)
	// register(b1) に対応します
	rootParams[3].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc(
		0, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init(
		_countof(rootParams), rootParams,
		1, &samplerDesc // サンプラーを追加
	);

	Microsoft::WRL::ComPtr<ID3DBlob> sigBlob, errBlob;
	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
	dxCommon_->GetDevice()->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));

	// ComputeShaderコンパイル
	auto csBlob = dxCommon_->CompileSharder(csPath, L"cs_6_0");

	// PSO設定
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = rootSignature_.Get();
	psoDesc.CS = { csBlob->GetBufferPointer(), csBlob->GetBufferSize() };
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	dxCommon_->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&computePipelineState_));

	dxCommon_->CreateSamplerHeap(); // サンプラーヒープの作成
}

void PostProcessManager::Draw(D3D12_GPU_DESCRIPTOR_HANDLE inputTexture)
{
	auto* cmdList = dxCommon_->GetCommandList();

	// エフェクトが空の場合は何もしない
	if (postEffects_.empty())
	{
		// スワップチェインのRTVに描画
		UINT backBufferIndex = dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();
		D3D12_CPU_DESCRIPTOR_HANDLE swapchainRTV = dxCommon_->GetRTVHandle(backBufferIndex);
		fullScreenEffect_->Draw(inputTexture, swapchainRTV);
		return;
	}

	// PSOを設定
	cmdList->SetPipelineState(computePipelineState_.Get());
	cmdList->SetComputeRootSignature(rootSignature_.Get());

	// DescriptorHeapセット
	ID3D12DescriptorHeap* descriptorHeaps[] = {
		srvManager_->GetSrvHeap(),
		dxCommon_->GetSamplerHeap()
	};
	cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// 出力バッファの状態遷移
	if (tempTextureAState_ != D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			tempTextureA_.Get(),
			tempTextureAState_,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		cmdList->ResourceBarrier(1, &barrier);
		tempTextureAState_ = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}

	// 入力テクスチャ (SRV) をルートパラメータ0にセット
	cmdList->SetComputeRootDescriptorTable(0, inputTexture);

	// 出力テクスチャ (UAV) をルートパラメータ1にセット
	cmdList->SetComputeRootDescriptorTable(1, tempUAVA_);

	// 各エフェクトの定数バッファをセット
	for (size_t i = 0; i < postEffects_.size(); ++i)
	{
		postEffects_[i]->UpdateParameters();
		postEffects_[i]->Draw(); // 定数バッファのセットのみ
	}

	// コンピュートシェーダーを一度だけ実行
	UINT dispatchX = (WinApp::kClientWidth + 7) / 8;
	UINT dispatchY = (WinApp::kClientHeight + 7) / 8;
	cmdList->Dispatch(dispatchX, dispatchY, 1);

	// 出力バッファをピクセルシェーダーリソース状態に遷移
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		tempTextureA_.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	cmdList->ResourceBarrier(1, &barrier);
	tempTextureAState_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	// スワップチェインのRTVに描画
	UINT backBufferIndex = dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE swapchainRTV = dxCommon_->GetRTVHandle(backBufferIndex);

	// 最終的にフルスクリーン描画
	fullScreenEffect_->Draw(tempSRVA_, swapchainRTV);
}

void PostProcessManager::ImGuiUpdate()
{
	ImGui::Begin("PostProcessManager");
	for (auto& effect : postEffects_)
	{
		effect->ImGuiUpdate();
	}
	ImGui::End();
}
