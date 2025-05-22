#pragma once
#include "BasePostEffect.h"
#include "math/Vector3.h"

class VignetteEffect : public BasePostEffect
{
public:
    struct Parameters
    {
        int enabled = 1;          // エフェクトが有効かどうか (0または1)
        float intensity = 0.7f;         // ヴィネットの強さ（0.0～1.0）
        float radius = 0.4f;            // ヴィネットの半径（0.0～1.0）
        float softness = 0.3f;          // エッジの柔らかさ（0.01～1.0）
        Vector3 color = { 0.0f, 0.0f, 0.0f }; // ヴィネットの色（RGB）
        float padding = 0.0f;           // アライメント用
    };

    VignetteEffect();
    ~VignetteEffect() override;

    void ImGuiUpdate() override;

    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, PostProcessManager* postProcessManager) override;
    void Draw() override;
    void CopyDataToConstantBuffer(void* mappedData) override;
    void SetIntensity(const float& intensity);
	float GetIntensity() const { return params_.intensity; } // ヴィネットの強さを取得
    void SetRadius(const float& radius);
	float GetRadius() const { return params_.radius; } // ヴィネットの半径を取得
    void SetSoftness(const float& softness);
	float GetSoftness() const { return params_.softness; } // ヴィネットの柔らかさを取得
    void SetColor(const Vector3& color);
	Vector3 GetColor() const { return params_.color; } // ヴィネットの色を取得

private:
	Parameters params_; // ヴィネットエフェクトのパラメータ
};
