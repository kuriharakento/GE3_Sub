#include "FullScreenEffect.h"

#include "DirectXTex/d3dx12.h"
#include "engine/base/DirectXCommon.h"
#include "engine/manager/SrvManager.h"

FullScreenEffect::FullScreenEffect() = default;
FullScreenEffect::~FullScreenEffect() = default;

void FullScreenEffect::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, const std::wstring& vsPath, const std::wstring& psPath)
{
    dxCommon_ = dxCommon;
    srvManager_ = srvManager;

    CD3DX12_DESCRIPTOR_RANGE range{};
    range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE samplerRange{};
    samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

    CD3DX12_ROOT_PARAMETER rootParams[2]{};
    rootParams[0].InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParams[1].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc{};
    rootSigDesc.Init(_countof(rootParams), rootParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    Microsoft::WRL::ComPtr<ID3DBlob> sigBlob, errBlob;
    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
    dxCommon_->GetDevice()->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));

    auto vs = dxCommon_->CompileSharder(vsPath, L"vs_6_0");
    auto ps = dxCommon_->CompileSharder(psPath, L"ps_6_0");

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
    psoDesc.pRootSignature = rootSignature_.Get();
    psoDesc.RTVFormats[0] = /*DXGI_FORMAT_R8G8B8A8_UNORM;*/ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    psoDesc.NumRenderTargets = 1;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.InputLayout = { nullptr, 0 };
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN; // ← ここが重要
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    dxCommon_->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));

    // Samplerヒープの作成
    dxCommon_->CreateSamplerHeap();
}

void FullScreenEffect::Draw(D3D12_GPU_DESCRIPTOR_HANDLE inputSRV, D3D12_CPU_DESCRIPTOR_HANDLE outputRTV)
{
    // SRVヒープとSamplerヒープを両方指定
    ID3D12DescriptorHeap* heaps[] = {
        srvManager_->GetSrvHeap(),             // CBV/SRV/UAV用
        dxCommon_->GetSamplerHeap()      // Sampler用
    };
    dxCommon_->GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps); // 2つを一括バインド

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
   
    // ルートシグネチャ・パイプラインセット
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
    dxCommon_->GetCommandList()->SetPipelineState(pipelineState_.Get());

    // SRVをルートパラメータにセット（ルートインデックスは設計に合わせて修正）
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, inputSRV);
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, dxCommon_->GetSamplerDescriptorHandle());
	// RTVセット
    dxCommon_->GetCommandList()->OMSetRenderTargets(1, &outputRTV, FALSE, nullptr);
    // フルスクリーンクアッド描画
    
    dxCommon_->GetCommandList()->DrawInstanced(4, 1, 0, 0);
}
