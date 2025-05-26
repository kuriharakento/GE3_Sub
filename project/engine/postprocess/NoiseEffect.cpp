#include "NoiseEffect.h"

NoiseEffect::NoiseEffect()
{
    params_.intensity = 0.2f;
    params_.time = 0.0f;
    params_.grainSize = 1.0f;
    params_.luminanceAffect = 0.0f;
    params_.enabled = 0;
	isDirty_ = true; // 初期状態ではパラメータが変更されているとみなす
}

NoiseEffect::~NoiseEffect()
{
}

void NoiseEffect::SetIntensity(float intensity)
{
    if (params_.intensity != intensity)
    {
        params_.intensity = intensity;
        isDirty_ = true;
    }
}

void NoiseEffect::SetTime(float time)
{
    if (params_.time != time)
    {
        params_.time = time;
        isDirty_ = true;
    }
}

void NoiseEffect::SetGrainSize(float grainSize)
{
    if (params_.grainSize != grainSize)
    {
        params_.grainSize = grainSize;
        isDirty_ = true;
    }
}

void NoiseEffect::SetLuminanceAffect(float luminanceAffect)
{
    if (params_.luminanceAffect != luminanceAffect)
    {
        params_.luminanceAffect = luminanceAffect;
        isDirty_ = true;
    }
}

void NoiseEffect::SetEnabled(bool enabled)
{
    if (enabled_ != enabled)
    {
        enabled_ = enabled;
        isDirty_ = true;
    }
}

size_t NoiseEffect::GetConstantBufferSize() const
{
    return sizeof(Parameters);
}

void NoiseEffect::CopyDataToConstantBuffer(void* mappedData)
{
    Parameters* data = static_cast<Parameters*>(mappedData);
    data->intensity = params_.intensity;
    data->time = params_.time;
    data->grainSize = params_.grainSize;
    data->luminanceAffect = params_.luminanceAffect;
    data->enabled = enabled_ ? 1 : 0;
}
