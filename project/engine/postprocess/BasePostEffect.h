#pragma once
#include "IPostEffect.h"
#include <wrl.h>
#include <d3d12.h>
#include "engine/base/DirectXCommon.h"

// 基本的な機能を提供する抽象基底クラス
class BasePostEffect : public IPostEffect
{
public:
    BasePostEffect();
    virtual ~BasePostEffect();

    void Initialize(DirectXCommon* dxCommon) override;
    virtual void UpdateParameters() override;
    D3D12_GPU_VIRTUAL_ADDRESS GetConstantBufferAddress() override;

    void SetEnabled(bool enabled) override;
    bool IsEnabled() const override { return enabled_; }

protected:
    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
	bool enabled_ = false; // エフェクトが有効かどうか (0または1)
    bool isDirty_ = true; // パラメータが変更されたかのフラグ

    // 継承先で独自のパラメータ構造体を定義し、
    // このメソッドをオーバーライドしてバッファサイズを返す
    virtual size_t GetConstantBufferSize() const = 0;

    // 継承先でこのメソッドをオーバーライドして
    // データをコピーする処理を実装
    virtual void CopyDataToConstantBuffer(void* mappedData) = 0;
};