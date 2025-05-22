struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput main(uint id : SV_VertexID)
{
    float4 positions[3];
    float2 uvs[3];

    positions[0] = float4(-1.0, 1.0, 0.0, 1.0);
    positions[1] = float4(3.0, 1.0, 0.0, 1.0);
    positions[2] = float4(-1.0, -3.0, 0.0, 1.0);

    uvs[0] = float2(0.0, 0.0);
    uvs[1] = float2(2.0, 0.0);
    uvs[2] = float2(0.0, 2.0);

    VSOutput o;
    o.pos = positions[id];
    o.uv = uvs[id];

    return o;
}