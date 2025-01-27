#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t3 padding;
    float32_t4x4 uvTransform;
    float32_t shininess;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

struct Camera
{
    float32_t3 worldPos;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    //テクスチャUV
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    //テクスチャカラー
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	//テクスチャの透明度が0以下の場合は描画しない
	if(textureColor.a == 0.0)
    {
        discard;
    }

    //Cameraへの方向
    float32_t3 toEye = normalize(gCamera.worldPos - input.worldPos);
    //入射角の反射ベクトル
    float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
    //outputカラー
    if (gMaterial.enableLighting != 0)
    { //Lightingする場合
        //拡散反射
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        //鏡面反射
        float RdotE = dot(reflectLight, toEye);
        float specularPow = pow(saturate(RdotE), gMaterial.shininess); //反射強度
        float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        //拡散反射・鏡面反射
        output.color.rgb = diffuse + specular;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    if (output.color.a == 0.0)
    {
        discard;
    }

    return output;
}