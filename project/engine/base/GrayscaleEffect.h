#pragma once
#include "BasePostEffect.h"
#include <string>

class GrayscaleEffect : public BasePostEffect
{
public:
    GrayscaleEffect();
    ~GrayscaleEffect() override;

    // グレースケール特有のパラメータ
    void SetIntensity(float intensity);
    float GetIntensity() const { return params_.intensity; }
    void SetEnabled(bool enabled) override;
protected:
    // 基底クラスのメソッドをオーバーライド
    size_t GetConstantBufferSize() const override;
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