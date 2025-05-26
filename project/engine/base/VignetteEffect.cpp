// VignetteEffect.cpp
#include "VignetteEffect.h"
#include <cassert>
#include "base/DirectXCommon.h"
#include "DirectXTex/d3dx12.h"
#include "manager/SrvManager.h"

VignetteEffect::VignetteEffect()
{
    // デフォルトパラメータ
    params_.intensity = 1.0f;
    params_.radius = 0.6f;
    params_.softness = 0.3f;
    params_.color = { 0.0f, 0.0f, 0.0f };
    params_.enabled = 0;
}

VignetteEffect::~VignetteEffect() {}

void VignetteEffect::SetIntensity(float intensity)
{
    if (params_.intensity != intensity)
    {
        params_.intensity = intensity;
        isDirty_ = true;
    }
}

void VignetteEffect::SetRadius(float radius)
{
    if (params_.radius != radius)
    {
        params_.radius = radius;
        isDirty_ = true;
    }
}

void VignetteEffect::SetSoftness(float softness)
{
    if (params_.softness != softness)
    {
        params_.softness = softness;
        isDirty_ = true;
    }
}

void VignetteEffect::SetColor(const Vector3& color)
{
    if (params_.color.x != color.x || params_.color.y != color.y || params_.color.z != color.z)
    {
        params_.color = color;
        isDirty_ = true;
    }
}

void VignetteEffect::SetEnabled(bool enabled)
{
    if (enabled_ != enabled)
    {
        enabled_ = enabled;
        isDirty_ = true;
    }
}

size_t VignetteEffect::GetConstantBufferSize() const
{
    // 256バイトアラインメント
    return (sizeof(Parameters) + 255) & ~255;
}

void VignetteEffect::CopyDataToConstantBuffer(void* mappedData)
{
    Parameters* data = static_cast<Parameters*>(mappedData);
	data->intensity = params_.intensity;
	data->radius = params_.radius;
	data->softness = params_.softness;
	data->color = params_.color;
	data->enabled = enabled_ ? 1 : 0; // 0または1で有効/無効を表現
}
