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
    float3 transformedUV = mul(float32_t4(input.texcoord,0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    //テクスチャの透明度が0以下の場合は描画しない
	if(textureColor.a == 0.0)
    {
        discard;
    }

	//ライティングを有効にしている場合はライティングを適用
	else if(gMaterial.enableLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial.color.a * textureColor.a;
        float32_t3 toEye = normalize(gCamera.worldPos - input.worldPos);
        float32_t3 reflecLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        float RdotE = dot(reflecLight, toEye);
        float speculaPow = pow(saturate(RdotE), gMaterial.shininess); //反射強度
        //拡散反射
        float32_t3 ddiffuse = gMaterial.color.rgb * textureColor.rgb * cos * gDirectionalLight.intensity;
        //鏡面反射
        float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * speculaPow * float32_t3(1.0f, 1.0f, 1.0f);
        //拡散反射 + 鏡面反射
        output.color.rgb = ddiffuse + specular;
        //アルファ値
        output.color.a = gMaterial.color.a * textureColor.a;
    }else
    {
        output.color = gMaterial.color * textureColor;
    }
    //Output.colorが透明度0以下の場合は描画しない
    if (output.color.a == 0.0)
    {
        discard;
    }

    return output;
}