struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput main(uint vertexID : SV_VertexID)
{
    VSOutput output;

    float2 positions[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0, 3.0),
        float2(3.0, -1.0)
    };
    float2 uvs[3] =
    {
        float2(0.0, 1.0),
        float2(0.0, -1.0),
        float2(2.0, 1.0)
    };

    output.pos = float4(positions[vertexID], 0.0, 1.0);
    output.uv = uvs[vertexID];
    return output;
}

