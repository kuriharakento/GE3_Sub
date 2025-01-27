#include "Object3d.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float3 padding;
    float4x4 uvTransform;
    float shininess;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

struct Camera
{
    float3 worldPos;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // テクスチャUVとカラーの取得
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    if (textureColor.a == 0.0)
    {
        discard;
    }

    if (gMaterial.enableLighting != 0)
    {
        // 法線とライト方向の正規化
        float3 normal = normalize(input.normal);
        float3 lightDir = normalize(-gDirectionalLight.direction);

        // 内積の計算と調整
        float NdotL = dot(normal, lightDir);
        NdotL = NdotL * 0.5f + 0.5f;
        NdotL = pow(NdotL, 2.0f); // 値を調整してライトの当たる範囲を広げる

        // 拡散反射の計算
        float3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;

        // 鏡面反射の計算
        float3 toEye = normalize(gCamera.worldPos - input.worldPos);
        float3 halfVector = normalize(lightDir + toEye);
        float NdotH = dot(normal, halfVector);
        NdotH = max(NdotH, 0.0f);
        float specularPow = pow(NdotH, gMaterial.shininess);
        float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow;

        // 最終的なカラーの計算
        output.color.rgb = diffuse + specular;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }

    if (output.color.a == 0.0f)
    {
        discard;
    }

    return output;
}
