#include "Grayscale.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer GrayscaleEffectParams : register(b0)
{
    float intensity; // グレースケールの強度 (0.0f～1.0f)
    int enabled; // エフェクトが有効かどうか (0または1)
    float2 padding; // パディング
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);

   // グレースケール変換 (エフェクトが有効な場合のみ)
    if (enabled > 0)
    {
        // RGB→グレースケール変換（ITU-R BT.709の輝度係数を使用）
        float gray = dot(output.color.rgb, float3(0.2125, 0.7154, 0.0721));
        
        // intensity パラメータによりエフェクトの強さを調整
        output.color.rgb = lerp(output.color.rgb, float3(gray, gray, gray), intensity);
    }
    
    return output;
}