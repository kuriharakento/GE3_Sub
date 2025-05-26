#include "PostEffect.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer GrayscaleEffectParams : register(b0)
{
    float intensity; // グレースケールの強度 (0.0f～1.0f)
    int enabled; // エフェクトが有効かどうか (0または1)
    float2 padding; // パディング
};

cbuffer VignetteEffectParams : register(b1)
{
    int vignetteEnabled; // ビネットエフェクトが有効かどうか (0または1)
    float vignetteIntensity; // ビネットの強度 (0.0f～1.0f)
    float vignetteRadius; // ビネットの半径 (0.0f～1.0f)
    float vignetteSoftness; // ビネットの柔らかさ (0.0f～1.0f)
    float3 vignetteColor; // ビネットの色 (RGB)
    float vignettePadding; // 16バイトアラインメント用
};

cbuffer NoiseEffectParams : register(b2)
{
    float noiseIntensity; // ノイズ強度
    float noiseTime; // 時間アニメーション用
    float grainSize; // 粒の大きさ（小さいと粗い、大きいと細かい）
    float luminanceAffect; // 明度に応じたノイズ強度（0=固定, 1=明度依存）
    int noiseEnabled; // 有効フラグ
    float3 noisePadding; // 16バイトアライメント
};

float random(float2 uv)
{
    return frac(sin(dot(uv.xy, float2(12.9898, 78.233)) + noiseTime) * 43758.5453);
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);

    // --- ノイズ処理 ---
    if (noiseEnabled != 0)
    {
        float2 grainUV = input.texcoord * grainSize + float2(noiseTime, noiseTime);
        float noiseValue = random(grainUV);

        // 明度による影響を乗算（luminanceAffect: 0.0 = 影響なし, 1.0 = 完全依存）
        float luminance = dot(output.color.rgb, float3(0.299, 0.587, 0.114));
        float luminanceFactor = lerp(1.0, luminance, saturate(luminanceAffect));

        float finalNoise = (noiseValue - 0.5f) * noiseIntensity * luminanceFactor;
        output.color.rgb += finalNoise;
    }
    
    // --- ヴィネット処理 ---
    if (vignetteEnabled != 0)
    {
        float2 center = float2(0.5, 0.5);
        float dist = distance(input.texcoord, center);
        
        // smoothstepの引数を逆にして、外側でvignetteが大きくなるように修正
        float vignette = smoothstep(vignetteRadius - vignetteSoftness, vignetteRadius, dist);
        
        // 中心ほど元の色、外側ほどcolorに補間
        output.color.rgb = lerp(output.color.rgb, vignetteColor, vignette * vignetteIntensity);
    }

    // --- グレースケール処理 ---
    if (enabled != 0)
    {
        float gray = dot(output.color.rgb, float3(0.2125, 0.7154, 0.0721));
        output.color.rgb = lerp(output.color.rgb, float3(gray, gray, gray), intensity);
    }
    
    //オバーフロー対策
    output.color.rgb = saturate(output.color.rgb);
    return output;
}