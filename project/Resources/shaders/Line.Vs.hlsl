#include "Line.hlsli"

cbuffer Transform : register(b0)
{
    float4x4 viewProjectionMatrix;
}

PSInput main(VSInput input)
{
    PSInput output;
    output.position = mul(viewProjectionMatrix, float4(input.position, 1.0));
    output.color = input.color;
    return output;
}
