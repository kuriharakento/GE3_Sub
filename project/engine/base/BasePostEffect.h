#pragma once
#include "IPostEffect.h"
#include <wrl.h>
#include <d3d12.h>
class PostProcessManager;
class DirectXCommon;
class SrvManager;

// 基本的な機能を提供する抽象基底クラス
class BasePostEffect
{
public:
    BasePostEffect();
	virtual ~BasePostEffect() = default;

    virtual void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, PostProcessManager* postProcessManager);
    virtual void Draw() = 0;
    virtual void ImGuiUpdate() {};

    void UpdateParameters() ;

    void SetEnabled(bool enabled);
    bool IsEnabled() const { return enabled_; }

protected:
    DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	PostProcessManager* postProcessManager_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;

	bool enabled_ = false; // エフェクトが有効かどうか (0または1)
    bool isDirty_ = true; // パラメータが変更されたかのフラグ

    // 継承先でこのメソッドをオーバーライドして
    // データをコピーする処理を実装
    virtual void CopyDataToConstantBuffer(void* mappedData) = 0;
};