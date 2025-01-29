#include "Object3d.hlsli"

// マテリアル
struct Material
{
    float4 color;
    int enableLighting;
    float3 padding;
    float4x4 uvTransform;
    float shininess;
};

// ディレクショナルライト
struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

// カメラ
struct Camera
{
    float3 worldPos;
};

// ポイントライト
struct GPUPointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
};

// スポットライト
struct GPUSpotLight
{
    float4 color;
    float3 position;
    float intensity;
    float3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloffStart;
};

struct LightCounts
{
    uint gPointLightCount;
    uint gSpotLightCount;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
StructuredBuffer<GPUPointLight> gPointLights : register(t3);
StructuredBuffer<GPUSpotLight> gSpotLights : register(t4);
ConstantBuffer<LightCounts> gLightCounts : register(b5);

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
        // 法線
        float3 normal = normalize(input.normal);

        // ポイントライトの合計
        float3 totalDiffuse = float3(0.0f, 0.0f, 0.0f);
        float3 totalSpecular = float3(0.0f, 0.0f, 0.0f);

        // スポットライトの合計
        float3 spotTotalDiffuse = float3(0.0f, 0.0f, 0.0f);
        float3 spotTotalSpecular = float3(0.0f, 0.0f, 0.0f);

        /*-----[ ディレクショナルライト ]-----*/

        // ライトの方向
        float3 lightDir = normalize(-gDirectionalLight.direction);

        // 内積の計算と調整
        float NdotL = dot(normal, lightDir);
        NdotL = NdotL * 0.5f + 0.5f;
        NdotL = pow(NdotL, 2.0f); // 値を調整してライトの当たる範囲を広げる

        // 拡散反射の計算
        float3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;

        // 鏡面反射の計算
        float3 toEye = normalize(gCamera.worldPos - input.worldPos);
        float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float NdotH = dot(normal, halfVector);
        NdotH = max(NdotH, 0.0f);
        float specularPow = pow(NdotH, gMaterial.shininess);
        float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow;

        /*-----[ ポイントライト ]-----*/

        for (uint j = 0; j < gLightCounts.gPointLightCount; j++)
        {
            // 入射光を計算
            float3 pointLightDir = normalize(input.worldPos - gPointLights[j].position);
            // 逆二乗の法則による減衰
            float distance = length(input.worldPos - gPointLights[j].position); // ポイントライトとの距離
            float factor = pow(saturate(-distance / gPointLights[j].radius + 1.0f), gPointLights[j].decay);
            // 内積の計算と調整
            float pointNdotL = dot(normal, -pointLightDir);
            pointNdotL = pointNdotL * 0.5f + 0.5f;
            pointNdotL = pow(pointNdotL, 2.0f);
            // 拡散反射の計算
            float3 pointDiffuse = gMaterial.color.rgb * textureColor.rgb * gPointLights[j].color.rgb * pointNdotL * gPointLights[j].intensity * factor;
            // 鏡面反射の計算
            float3 pointHalfVector = normalize(-pointLightDir + toEye);
            float pointNdotH = dot(normal, pointHalfVector);
            pointNdotH = max(pointNdotH, 0.0f);
            float pointSpecularPow = pow(pointNdotH, gMaterial.shininess);
            float3 pointSpecular = gPointLights[j].color.rgb * gPointLights[j].intensity * pointSpecularPow * factor;
            totalDiffuse += pointDiffuse;
            totalSpecular += pointSpecular;
        }

        /*-----[ スポットライト ]-----*/

        for (uint k = 0; k < gLightCounts.gSpotLightCount; k++)
        {
            // 入射光を計算
            float3 spotLightDirOnSurface = normalize(input.worldPos - gSpotLights[k].position);
            // 逆二乗の法則による減衰
            float spotDistance = length(gSpotLights[k].position - input.worldPos); // スポットライトとの距離
            float spotFactor = pow(saturate(-spotDistance / gSpotLights[k].distance + 1.0f), gSpotLights[k].decay);
            // フォールオフ
            float cosAngle = dot(spotLightDirOnSurface, gSpotLights[k].direction);
            float falloffFactor = saturate((cosAngle - gSpotLights[k].cosAngle) / (gSpotLights[k].cosFalloffStart - gSpotLights[k].cosAngle));
            // 内積の計算と調整
            float spotNdotL = dot(normal, -spotLightDirOnSurface);
            spotNdotL = spotNdotL * 0.5f + 0.5f;
            spotNdotL = pow(spotNdotL, 2.0f);
            // 拡散反射の計算
            float3 spotDiffuse = gMaterial.color.rgb * textureColor.rgb * gSpotLights[k].color.rgb * spotNdotL * gSpotLights[k].intensity * spotFactor * falloffFactor;
            // 鏡面反射の計算
            float3 spotHalfVector = normalize(-spotLightDirOnSurface + toEye);
            float spotNdotH = dot(normal, spotHalfVector);
            spotNdotH = max(spotNdotH, 0.0f);
            float spotSpecularPow = pow(spotNdotH, gMaterial.shininess);
            float3 spotSpecular = gSpotLights[k].color.rgb * gSpotLights[k].intensity * spotSpecularPow * spotFactor * falloffFactor;
            spotTotalDiffuse += spotDiffuse;
            spotTotalSpecular += spotSpecular;
        }

        /*-----[ 結果の合成 ]-----*/

        output.color.rgb = specular + diffuse + totalSpecular + totalDiffuse + spotTotalSpecular + spotTotalDiffuse;
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
