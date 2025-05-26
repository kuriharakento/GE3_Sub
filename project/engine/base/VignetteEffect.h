// VignetteEffect.h
#pragma once
#include "BasePostEffect.h"
#include <string>

#include "math/Vector3.h"

class VignetteEffect : public BasePostEffect
{
public:
    VignetteEffect();
    ~VignetteEffect() override;

    // ビネット特有のパラメータ
    void SetIntensity(float intensity);
    float GetIntensity() const { return params_.intensity; }
    void SetRadius(float radius);
    float GetRadius() const { return params_.radius; }
    void SetSoftness(float softness);
    float GetSoftness() const { return params_.softness; }
    void SetColor(const Vector3& color);
    Vector3 GetColor() const { return params_.color; }
    void SetEnabled(bool enabled) override;

protected:
    // 基底クラスのメソッドをオーバーライド
    size_t GetConstantBufferSize() const override;
    void CopyDataToConstantBuffer(void* mappedData) override;

private:
    struct Parameters
    {
        int enabled;         // ビネットエフェクトが有効かどうか (0または1)
        float intensity;     // ビネットの強度 (0.0f～1.0f)
        float radius;        // ビネットの半径 (0.0f～1.0f)
        float softness;      // ビネットの柔らかさ (0.0f～1.0f)
        Vector3 color;       // ビネットの色 (RGB)
        float padding;       // 16バイトアラインメント用
    };

    Parameters params_;
};
