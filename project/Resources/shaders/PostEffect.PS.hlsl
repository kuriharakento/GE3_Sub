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

    //int crtEnabled;
    //int scanlineEnabled;
    //float scanlineIntensity;
    //float scanlineCount;

    //int distortionEnabled;
    //float distortionStrength;
    //int chromAberrationEnabled;
    //float chromAberrationOffset;

    //float4 pad3;
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

        float vignette = smoothstep(vignetteRadius - vignetteSoftness, vignetteRadius, dist);
        output.color.rgb = lerp(output.color.rgb, vignetteColor, vignette * vignetteIntensity);
    }

    // --- グレースケール処理 ---
    if (grayscaleEnabled != 0)
    {
        float gray = dot(output.color.rgb, float3(0.2125, 0.7154, 0.0721));
        output.color.rgb = lerp(output.color.rgb, float3(gray, gray, gray), grayscaleIntensity);
    }

    // オバーフロー対策
    output.color.rgb = saturate(output.color.rgb);
    return output;
}
