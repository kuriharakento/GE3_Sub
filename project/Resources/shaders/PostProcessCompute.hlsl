// Constants for Vignette Effect
// register(b0) はビネットエフェクト専用
cbuffer VignetteParams : register(b0)
{
    int vignetteEnabled;
    float vignetteIntensity;
    float vignetteRadius;
    float vignetteSoftness;
    float3 vignetteColor;
    float paddingV; // 16バイトアライメントのためのパディング
};

// Constants for Grayscale Effect
// register(b1) はグレースケールエフェクト専用
cbuffer GrayscaleParams : register(b1)
{
    int grayscaleEnabled;
    float grayscaleIntensity;
    float paddingG0; // 16バイトアライメントのためのパディング
    float paddingG1; // 16バイトアライメントのためのパディング
};

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);
SamplerState samplerLinear : register(s0);

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint width, height;
    inputTexture.GetDimensions(width, height);

    // スレッドIDがテクスチャの範囲外であれば処理を終了
    if (dispatchThreadID.x >= width || dispatchThreadID.y >= height)
    {
        return;
    }

    // ピクセル座標とUV座標を計算
    float2 uv = float2(dispatchThreadID.xy) / float2(width, height);
    uint2 pixelCoord = uint2(dispatchThreadID.xy);

    // 入力テクスチャから現在のピクセルの色をロード
    // Compute Shaderでは通常Loadを使用し、正確なテクセルを取得します
    float4 color = inputTexture.Load(uint3(pixelCoord, 0));

    // --- Vignette処理 ---
    // VignetteParams cbufferからパラメータを参照
    if (vignetteEnabled != 0) // エフェクトが有効な場合のみ適用
    {
        float2 center = float2(0.5, 0.5); // スクリーンの中心
        float dist = distance(uv, center); // 中心からの距離を計算

        // smoothstepを使って、ビネットの開始と終了を滑らかにする
        float vignette = smoothstep(vignetteRadius - vignetteSoftness, vignetteRadius, dist);

        // vignetteの値に基づいて、元の色からビネットの色へ lerp します
        color.rgb = lerp(color.rgb, vignetteColor.rgb, vignette * vignetteIntensity);
    }

    // --- Grayscale処理 ---
    if (grayscaleEnabled != 0)
    {
        // 輝度（グレー値）を計算 (NTSC係数を使用)
        float gray = dot(color.rgb, float3(0.2125, 0.7154, 0.0721));

        
        color.rgb = lerp(color.rgb, float3(gray, gray, gray), grayscaleIntensity);
    }

    // 最終的な色を出力テクスチャに書き込む
    outputTexture[dispatchThreadID.xy] = color;
}