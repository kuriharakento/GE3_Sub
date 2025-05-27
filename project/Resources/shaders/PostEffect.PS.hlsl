// スマート色収差最適化 - 適応的サンプリング
#include "PostEffect.hlsli"
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer PostEffectParams : register(b0)
{
    float grayscaleIntensity;
    int grayscaleEnabled;
    float2 pad0;
    int vignetteEnabled;
    float vignetteIntensity;
    float vignetteRadius;
    float vignetteSoftness;
    float3 vignetteColor;
    float pad1;
    int noiseEnabled;
    float noiseIntensity;
    float noiseTime;
    float grainSize;
    float luminanceAffect;
    float3 pad2;
    int crtEnabled;
    int scanlineEnabled;
    float scanlineIntensity;
    float scanlineCount;
    int distortionEnabled;
    float distortionStrength;
    int chromAberrationEnabled;
    float chromAberrationOffset;
    float4 pad3;
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float2 uv = input.texcoord;
    
    // 係数の事前計算（分岐回避）
    float crtFactor = (float) crtEnabled;
    float chromaticActive = crtFactor * (float) chromAberrationEnabled;
    float distortActive = crtFactor * (float) distortionEnabled;
    float scanlineActive = crtFactor * (float) scanlineEnabled;
    
    // 歪み（線形補間で分岐回避）
    float2 centerOffset = uv - 0.5;
    float distortStrength = distortActive * distortionStrength * dot(centerOffset, centerOffset);
    uv += centerOffset * distortStrength;
    
    // 色収差の最適化サンプリング
    float2 chromaticOffset = float2(chromAberrationOffset * 0.001, 0.0);
    
    // 条件付きサンプリング（分岐は避けられないが最小化）
    float3 color;
    [branch]
    if (chromaticActive > 0.0)
    {
        // 効率的な3サンプル戦略
        float3 centerColor = gTexture.Sample(gSampler, uv).rgb;
        float redShift = gTexture.Sample(gSampler, uv + chromaticOffset).r;
        float blueShift = gTexture.Sample(gSampler, uv - chromaticOffset).b;
        
        // 係数による混合（完全置換ではなく混合で自然な見た目）
        color = float3(
            lerp(centerColor.r, redShift, chromaticActive),
            centerColor.g,
            lerp(centerColor.b, blueShift, chromaticActive)
        );
    }
    else
    {
        color = gTexture.Sample(gSampler, uv).rgb;
    }
    
    // 残りのエフェクトを統合処理（分岐完全排除）
    float luminance = dot(color, float3(0.299, 0.587, 0.114));
    
    // 走査線
    float scanPattern = step(0.5, frac(uv.y * scanlineCount));
    float scanEffect = lerp(1.0, lerp(0.7, 1.0, scanPattern), scanlineActive * scanlineIntensity);
    color *= scanEffect;
    
    // ノイズ
    float noise = frac(uv.x * 12.9898 + uv.y * 78.233 + noiseTime) - 0.5;
    float noiseAmount = (float) noiseEnabled * noiseIntensity;
    float lumFactor = lerp(1.0, luminance, saturate(luminanceAffect));
    color += noise * noiseAmount * lumFactor;
    
    // ビネット（二乗距離使用）
    float vignetteDistSq = dot(centerOffset, centerOffset);
    float vignetteEffect = saturate((vignetteDistSq - vignetteRadius * vignetteRadius) /
                                   (vignetteSoftness * vignetteSoftness));
    float vignetteAmount = (float) vignetteEnabled * vignetteIntensity;
    color = lerp(color, vignetteColor, vignetteEffect * vignetteAmount);
    
    // グレースケール
    float grayAmount = (float) grayscaleEnabled * grayscaleIntensity;
    color = lerp(color, luminance.xxx, grayAmount);
    
    PixelShaderOutput output;
    output.color = float4(saturate(color), 1.0);
    return output;
}