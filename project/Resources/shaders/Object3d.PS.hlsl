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
struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
};

//スポットライト
struct SpotLight
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

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);

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

        /*-----[ ディレクショナルライト ]-----*/

        //ライトの方向
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

    	//入射光を計算
        float3 pointLightDir = normalize(input.worldPos - gPointLight.position);

    	//逆二乗の法則による減衰
        float distance = length(input.worldPos - gPointLight.position); //ポイントライトとの距離
        float factor = pow(saturate(-distance / gPointLight.radius + 1.0f),gPointLight.decay);

    	//内積の計算と調整
        float pointNdotL = dot(normal, -pointLightDir);
        pointNdotL = pointNdotL * 0.5f + 0.5f;
        pointNdotL = pow(pointNdotL, 2.0f);

        //拡散反射の計算
        float3 pointDiffuse = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * pointNdotL * gPointLight.intensity * factor;

    	//鏡面反射の計算
        float3 pointHalfVector = normalize(-pointLightDir + toEye);
        float pointNdotH = dot(normal, pointHalfVector);
        pointNdotH = max(pointNdotH, 0.0f);
        float pointSpecularPow = pow(pointNdotH, gMaterial.shininess);
        float3 pointSpecular = gPointLight.color.rgb * gPointLight.intensity * pointSpecularPow * factor;

        /*-----[ スポットライト ]-----*/

        //入射光を計算
        float3 spotLightDirOnSurface = normalize(input.worldPos - gSpotLight.position);

       //逆二乗の法則による減衰
        float spotDistance = length(gSpotLight.position - input.worldPos); //スポットライトとの距離
        float spotFactor = pow(saturate(-spotDistance / gSpotLight.distance + 1.0f), gSpotLight.decay);

    	//フォールオフ
        float cosAngle = dot(spotLightDirOnSurface, gSpotLight.direction);
        float falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));

        //内積の計算と調整
        float spotNdotL = dot(normal, -spotLightDirOnSurface);
        spotNdotL = spotNdotL * 0.5f + 0.5f;
        spotNdotL = pow(spotNdotL, 2.0f);

        //拡散反射の計算
        float3 spotDiffuse = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * spotNdotL * gSpotLight.intensity * spotFactor * falloffFactor;

        //鏡面反射の計算
        float3 spotHalfVector = normalize(-spotLightDirOnSurface + toEye);
        float spotNdotH = dot(normal, spotHalfVector);
        spotNdotH = max(spotNdotH, 0.0f);
        float spotSpecularPow = pow(spotNdotH, gMaterial.shininess);
        float3 spotSpecular = gSpotLight.color.rgb * gSpotLight.intensity * spotSpecularPow * spotFactor * falloffFactor;
       
        /*-----[ 結果の合成 ]-----*/

        output.color.rgb = diffuse + specular + pointDiffuse + pointSpecular + spotDiffuse + spotSpecular;
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
