#include "PostEffect.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer GrayscaleEffectParams : register(b0)
{
    float intensity;                    // グレースケールの強度 (0.0f～1.0f)
    int enabled;                        // エフェクトが有効かどうか (0または1)
    float2 padding;                     // パディング
};

cbuffer VignetteEffectParams : register(b1)
{
    int vignetteEnabled;                // ビネットエフェクトが有効かどうか (0または1)
    float vignetteIntensity;            // ビネットの強度 (0.0f～1.0f)
    float vignetteRadius;               // ビネットの半径 (0.0f～1.0f)
    float vignetteSoftness;             // ビネットの柔らかさ (0.0f～1.0f)
    float3 vignetteColor;               // ビネットの色 (RGB)
    float vignettePadding;              // 16バイトアラインメント用
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);

    // --- ヴィネット処理 ---
    if (vignetteEnabled > 0)
    {
        float2 center = float2(0.5, 0.5);
        float dist = distance(input.texcoord, center);
        
        // smoothstepの引数を逆にして、外側でvignetteが大きくなるように修正
        float vignette = smoothstep(vignetteRadius - vignetteSoftness, vignetteRadius , dist);
        
        // 中心ほど元の色、外側ほどcolorに補間
        output.color.rgb = lerp(output.color.rgb, vignetteColor, vignette * vignetteIntensity);
    }

    // --- グレースケール処理 ---
    if (enabled > 0)
    {
        float gray = dot(output.color.rgb, float3(0.2125, 0.7154, 0.0721));
        output.color.rgb = lerp(output.color.rgb, float3(gray, gray, gray), intensity);
    }
    
    
    return output;
}