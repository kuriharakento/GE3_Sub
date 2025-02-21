#include "LightCommon.h"
#include "DirectXTex/d3dx12.h"
#include <d3dcompiler.h>
#include <cassert>

LightCommon::LightCommon(DirectXCommon* dxCommon)
    : dxCommon_(dxCommon)
{
}

LightCommon::~LightCommon()
{
    //リソースの開放
    if (shadowMap_) shadowMap_->Release();
    if (dsvHeapShadowMap_) dsvHeapShadowMap_->Release();
}

void LightCommon::CreateShadowMapPSO()
{
    // シャドウマップ用のルートシグネチャを生成
    CreateShadowMapRootSignature();

    // シャドウマップ用のシェーダーをコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileSharder(L"Resources/shaders/ShadowMap.VS.hlsl", L"vs_6_0");
    assert(vertexShaderBlob != nullptr);

    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon_->CompileSharder(L"Resources/shaders/ShadowMap.PS.hlsl", L"ps_6_0");
    assert(pixelShaderBlob != nullptr);

    // ブレンドステートの設定
    D3D12_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE, FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
    {
        blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
    }

    // ラスタライザーステートの設定
    D3D12_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.ForcedSampleCount = 0;
    rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // 深度ステンシルステートの設定
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    // PSOの設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc = {};
    graphicsPipelineStateDesc.pRootSignature = shadowMapRootSignature_.Get();
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
    graphicsPipelineStateDesc.BlendState = blendDesc;
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&shadowMapPSO_));
    assert(SUCCEEDED(hr));
}

void LightCommon::CreateShadowMapRootSignature()
{
    // ルートシグネチャの設定
    D3D12_ROOT_PARAMETER rootParameters[1];
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    rootParameters[0].Descriptor.RegisterSpace = 0;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    Microsoft::WRL::ComPtr<ID3DBlob> signature;
    Microsoft::WRL::ComPtr<ID3DBlob> error;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    assert(SUCCEEDED(hr));

    hr = dxCommon_->GetDevice()->CreateRootSignature(
        0, 
        signature->GetBufferPointer(), 
        signature->GetBufferSize(),
        IID_PPV_ARGS(&shadowMapRootSignature_)
    );
    assert(SUCCEEDED(hr));
}

void LightCommon::CreateShadowMap()
{
    HRESULT hr;

    // シャドウマップのサイズ
    const UINT shadowMapWidth = 1024;
    const UINT shadowMapHeight = 1024;

    // シャドウマップ用の深度バッファリソースの設定
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Width = shadowMapWidth;
    resourceDesc.Height = shadowMapHeight;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&shadowMap_)
    );
    assert(SUCCEEDED(hr));

    // シャドウマップ用のDSVヒープの設定
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    hr = dxCommon_->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeapShadowMap_));
    assert(SUCCEEDED(hr));

    // シャドウマップ用のDSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dxCommon_->GetDevice()->CreateDepthStencilView(shadowMap_.Get(), &dsvDesc, dsvHeapShadowMap_->GetCPUDescriptorHandleForHeapStart());

    // ビューポートとシザー矩形の設定
    viewportShadowMap_.TopLeftX = 0.0f;
    viewportShadowMap_.TopLeftY = 0.0f;
    viewportShadowMap_.Width = static_cast<float>(shadowMapWidth);
    viewportShadowMap_.Height = static_cast<float>(shadowMapHeight);
    viewportShadowMap_.MinDepth = 0.0f;
    viewportShadowMap_.MaxDepth = 1.0f;

    scissorRectShadowMap_.left = 0;
    scissorRectShadowMap_.top = 0;
    scissorRectShadowMap_.right = shadowMapWidth;
    scissorRectShadowMap_.bottom = shadowMapHeight;
}

void LightCommon::PreDrawShadowMap()
{
    // シャドウマップの描画前処理
    dxCommon_->GetCommandList()->RSSetViewports(1, &viewportShadowMap_);
    dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRectShadowMap_);

    // シャドウマップ用のDSVを設定
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeapShadowMap_->GetCPUDescriptorHandleForHeapStart();
    dxCommon_->GetCommandList()->OMSetRenderTargets(0, nullptr, FALSE, &dsvHandle);

    // シャドウマップのクリア
    dxCommon_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void LightCommon::PostDrawShadowMap()
{
    // シャドウマップの描画後処理
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = shadowMap_.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
}

ID3D12PipelineState* LightCommon::GetShadowMapPSO() const
{
    return shadowMapPSO_.Get();
}

ID3D12RootSignature* LightCommon::GetShadowMapRootSignature() const
{
    return shadowMapRootSignature_.Get();
}
