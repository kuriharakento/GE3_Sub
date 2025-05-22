#pragma once
#include "BasePostEffect.h"
#include <string>

class GrayscaleEffect : public BasePostEffect
{
public:
    GrayscaleEffect();
    ~GrayscaleEffect() override;
    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, PostProcessManager* postProcessManager) override;
    void Draw() override;
	void ImGuiUpdate() override;

    // グレースケール特有のパラメータ
    void SetIntensity(float intensity);
    float GetIntensity() const { return params_.intensity; }
protected:
    // 基底クラスのメソッドをオーバーライド
    void CopyDataToConstantBuffer(void* mappedData) override;

private:
    struct Parameters
    {
        int enabled;     // シェーダーの grayscaleEnabled に対応 (オフセット0)
        float intensity; // シェーダーの grayscaleIntensity に対応 (オフセット4)
        float padding[2]; // 残りのパディング (オフセット8, 12)
    };

    Parameters params_;
};