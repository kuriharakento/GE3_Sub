cbuffer VignetteParams : register(b0)
{
    int enabled; // エフェクトが有効かどうか (0または1)
    float intensity; // ヴィネットの強さ（0.0～1.0）
    float radius; // ヴィネットの半径（0.0～1.0）
    float softness; // エッジの柔らかさ（0.01～1.0）
    float3 color; // ヴィネットの色（RGB）
    float padding; // アライメント用
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(VSOutput input) : SV_TARGET
{
    float4 texColor = gTexture.Sample(gSampler, input.uv);
    
    if (enabled > 0)
    {
        float2 center = float2(0.5, 0.5);
        float dist = distance(input.uv, center);
        
        // smoothstepの引数を逆にして、外側でvignetteが大きくなるように修正
        float vignette = smoothstep(radius - softness, radius, dist);
        
        // 中心ほど元の色、外側ほどcolorに補間
        float3 result = lerp(texColor.rgb, color, vignette * intensity);
        return float4(result, texColor.a);
    }
    else
    {
        return texColor;
    }
}