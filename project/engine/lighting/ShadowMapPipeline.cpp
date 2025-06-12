#include "ShadowMapPipeline.h"

#include <cassert>
#include "base/Logger.h"
#include "base/DirectXCommon.h"
#include "DirectXTex/d3dx12.h"

void ShadowMapPipeline::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	// PSOの初期化
	CreateRootSignature();
	CreatePipelineState();
}

void ShadowMapPipeline::CreateRootSignature()
{
    // ルートパラメータの設定
    D3D12_ROOT_PARAMETER rootParameters[1] = {};

    // TransformationMatrix用の定数バッファ
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[0].Descriptor.ShaderRegister = 0;

    // ルートシグネチャの設定
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumParameters = _countof(rootParameters);

    // シリアライズ
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr))
    {
        Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    // ルートシグネチャの作成
    hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}

void ShadowMapPipeline::CreatePipelineState()
{
    // シェーダーのコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileShader(L"Resources/shaders/ShadowMap.VS.hlsl", L"vs_6_0");

    // 入力レイアウトの設定
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // パイプラインステートの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
    pipelineStateDesc.pRootSignature = rootSignature_.Get();
    pipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
    pipelineStateDesc.PS = {};
    pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    // 深度バイアスの設定（シャドウアクネ対策）
    pipelineStateDesc.RasterizerState.DepthBias = 1000;
    pipelineStateDesc.RasterizerState.DepthBiasClamp = 0.0f;
    pipelineStateDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;

    // 深度ステンシルの設定
    pipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    // 入力レイアウト
    pipelineStateDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // レンダーターゲット設定（シャドウマップは深度のみなのでRTVは不要）
    pipelineStateDesc.NumRenderTargets = 0;
    pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    pipelineStateDesc.SampleDesc.Count = 1;
    pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    // パイプラインステートの作成
    HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState_));
    assert(SUCCEEDED(hr));
}
