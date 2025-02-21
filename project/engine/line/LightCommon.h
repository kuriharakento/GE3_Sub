#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <string>
#include "base/DirectXCommon.h"

class LightCommon
{
public:
    // コンストラクタ
    LightCommon(DirectXCommon* dxCommon);
    ~LightCommon();
    // シャドウマップ用のPSOを生成
    void CreateShadowMapPSO();
    // シャドウマップ用のルートシグネチャを生成
    void CreateShadowMapRootSignature();
    // シャドウマップの生成
    void CreateShadowMap();
    // シャドウマップの描画前処理
    void PreDrawShadowMap();
    //　シャドウマップの描画後処理
    void PostDrawShadowMap();

public: // アクセッサ
    // シャドウマップ用のPSOを取得
    ID3D12PipelineState* GetShadowMapPSO() const;
    // シャドウマップ用のルートシグネチャを取得
    ID3D12RootSignature* GetShadowMapRootSignature() const;
    //dxCommonの取得
    DirectXCommon* GetDxCommon() { return dxCommon_; }

private:
    // DirectXCommon
    DirectXCommon* dxCommon_ = nullptr;
    // シャドウマップ用のPSO
    Microsoft::WRL::ComPtr<ID3D12PipelineState> shadowMapPSO_;
    // シャドウマップ用のルートシグネチャ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> shadowMapRootSignature_;
    //シャドウマップ用の深度バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> shadowMap_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeapShadowMap_;
    D3D12_VIEWPORT viewportShadowMap_;
    D3D12_RECT scissorRectShadowMap_;
};
