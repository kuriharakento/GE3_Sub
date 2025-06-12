// 座標変換行列（ライト視点での変換に使用）
cbuffer TransformationMatrix : register(b0)
{
    matrix WVP; // ワールド×ライトビュー×ライトプロジェクション行列
}

// 頂点シェーダー入力
struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

// 頂点シェーダー出力
struct VSOutput
{
    float4 position : SV_POSITION; // 射影座標
};

// メイン関数
VSOutput main(VSInput input)
{
    VSOutput output;
    
    // ライト視点でのクリップ空間座標を計算
    output.position = mul(input.position, WVP);
    
    return output;
}