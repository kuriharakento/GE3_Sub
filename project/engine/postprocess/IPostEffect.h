#pragma once
#include <d3d12.h>

class DirectXCommon;

class IPostEffect
{
public:
    virtual ~IPostEffect() = default;

    // 初期化処理
    virtual void Initialize(DirectXCommon* dxCommon) = 0;

    // パラメータの更新
    virtual void UpdateParameters() = 0;

    // 定数バッファの取得（シェーダーにバインドするため）
    virtual D3D12_GPU_VIRTUAL_ADDRESS GetConstantBufferAddress() = 0;

    // エフェクトの有効/無効
    virtual void SetEnabled(bool enabled) = 0;
    virtual bool IsEnabled() const = 0;
};

