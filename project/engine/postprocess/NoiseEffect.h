#pragma once
#include "BasePostEffect.h"

class NoiseEffect : public BasePostEffect
{
public:
    NoiseEffect();
    ~NoiseEffect() override;

    void SetIntensity(float intensity);
    void SetTime(float time);
    void SetGrainSize(float grainSize);
    void SetLuminanceAffect(float luminanceAffect);
    void SetEnabled(bool enabled) override;

    float GetIntensity() const { return params_.intensity; }
    float GetTime() const { return params_.time; }
    float GetGrainSize() const { return params_.grainSize; }
    float GetLuminanceAffect() const { return params_.luminanceAffect; }

protected:
    size_t GetConstantBufferSize() const override;
    void CopyDataToConstantBuffer(void* mappedData) override;

private:
    struct Parameters
    {
        float intensity;
        float time;
        float grainSize;
        float luminanceAffect;
        int enabled;
        float padding[3];
    };

    Parameters params_;
};
