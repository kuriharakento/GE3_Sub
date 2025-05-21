#include "GrayscaleEffect.h"
#include <cassert>

GrayscaleEffect::GrayscaleEffect()
{
    // デフォルトパラメータの設定
    params_.intensity = 1.0f;
	params_.enabled = 0;  // エフェクトは無効
}

GrayscaleEffect::~GrayscaleEffect()
{
    
}

void GrayscaleEffect::SetIntensity(float intensity)
{
    // 値が変更された場合のみ更新フラグを立てる
    if (params_.intensity != intensity)
    {
        params_.intensity = intensity;
        isDirty_ = true;  // 基底クラスのフラグを使用
    }
}

void GrayscaleEffect::SetEnabled(bool enabled)
{
	if (enabled_ != enabled)
	{
		enabled_ = enabled;
		isDirty_ = true;  // 基底クラスのフラグを使用
	}
}

size_t GrayscaleEffect::GetConstantBufferSize() const
{
    return sizeof(Parameters);
}

void GrayscaleEffect::CopyDataToConstantBuffer(void* mappedData)
{
    // 定数バッファにデータをコピー
    Parameters* data = static_cast<Parameters*>(mappedData);
    data->intensity = params_.intensity;
    data->enabled = enabled_ ? 1 : 0;  // 基底クラスのenabledフラグを使用
}