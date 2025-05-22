#pragma once
#include <d3d12.h>
#include <string>

class SrvManager;
class DirectXCommon;

class IPostEffect
{
public:
    virtual ~IPostEffect() = default;
    // パラメータの更新
    virtual void UpdateParameters() = 0;

    // 定数バッファの取得（シェーダーにバインドするため）
    virtual D3D12_GPU_VIRTUAL_ADDRESS GetConstantBufferAddress() = 0;

    // エフェクトの有効/無効
    virtual void SetEnabled(bool enabled) = 0;
    virtual bool IsEnabled() const = 0;
};

