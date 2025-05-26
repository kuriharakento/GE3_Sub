#include "PostProcessManager.h"
#include "PostProcessManager.h"
#include "engine/base/DirectXCommon.h"
#include "engine/manager/SrvManager.h"
#include "DirectXTex/d3dx12.h"
#include <cassert>

PostProcessManager::PostProcessManager() {}

PostProcessManager::~PostProcessManager() {}

void PostProcessManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, const std::wstring& vsPath, const std::wstring& psPath)
{
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	SetupPipeline(vsPath, psPath);

	grayscaleEffect_ = std::make_unique<GrayscaleEffect>();
	grayscaleEffect_->Initialize(dxCommon);
	vignetteEffect_ = std::make_unique<VignetteEffect>();
	vignetteEffect_->Initialize(dxCommon);
}

void PostProcessManager::SetupPipeline(const std::wstring& vsPath, const std::wstring& psPath)
{
	// ルートシグネチャの作成
	CD3DX12_DESCRIPTOR_RANGE srvRange{};
	srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE samplerRange{};
	samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

	CD3DX12_ROOT_PARAMETER rootParams[4]{};
	rootParams[0].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParams[1].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);
	// グレイスケールエフェクトの定数バッファ
	rootParams[2].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
	// ビネットエフェクトの定数バッファ
	rootParams[3].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.Init(_countof(rootParams), rootParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> sigBlob, errBlob;
	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
	dxCommon_->GetDevice()->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));

	// パイプラインステートの作成
	auto vs = dxCommon_->CompileSharder(vsPath, L"vs_6_0");
	auto ps = dxCommon_->CompileSharder(psPath, L"ps_6_0");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
	psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
	psoDesc.pRootSignature = rootSignature_.Get();
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.InputLayout = { nullptr, 0 };
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	dxCommon_->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));
}

void PostProcessManager::Draw(D3D12_GPU_DESCRIPTOR_HANDLE inputTexture)
{
	auto cmdList = dxCommon_->GetCommandList();

	cmdList->SetPipelineState(pipelineState_.Get());
	cmdList->SetGraphicsRootSignature(rootSignature_.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// SRVヒープとSamplerヒープを両方指定
	ID3D12DescriptorHeap* heaps[] = {
		srvManager_->GetSrvHeap(),
		dxCommon_->GetSamplerHeap()
	};
	cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

	cmdList->SetGraphicsRootDescriptorTable(0, inputTexture);
	cmdList->SetGraphicsRootDescriptorTable(1, dxCommon_->GetSamplerDescriptorHandle());

	//グレイスケールの描画
	grayscaleEffect_->UpdateParameters();
	cmdList->SetGraphicsRootConstantBufferView(2, grayscaleEffect_->GetConstantBufferAddress());

	//ビネットの描画
	vignetteEffect_->UpdateParameters();
	cmdList->SetGraphicsRootConstantBufferView(3, vignetteEffect_->GetConstantBufferAddress());

	cmdList->DrawInstanced(3, 1, 0, 0);
}
