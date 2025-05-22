#pragma once
#include "BasePostEffect.h"
#include <string>

class GrayscaleEffect : public BasePostEffect
{
public:
    GrayscaleEffect();
    ~GrayscaleEffect() override;

    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, const std::wstring& vsPath, const std::wstring& psPath) override;
    void Draw(D3D12_GPU_DESCRIPTOR_HANDLE inputSRV, D3D12_CPU_DESCRIPTOR_HANDLE outputRTV) override;

    // グレースケール特有のパラメータ
    void SetIntensity(float intensity);
    float GetIntensity() const { return params_.intensity; }
protected:
    // 基底クラスのメソッドをオーバーライド
    void CopyDataToConstantBuffer(void* mappedData) override;

private:
    struct Parameters
    {
        float intensity;    // グレースケールの強度 (0.0f～1.0f)
        int enabled;        // エフェクトが有効かどうか (0または1)
        float padding[2];   // 16バイトアラインメントのためのパディング
    };

    Parameters params_;
};