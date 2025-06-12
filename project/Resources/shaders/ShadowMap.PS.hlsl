// ピクセルシェーダー入力
struct PSInput
{
    float4 position : SV_POSITION;
};

// メイン関数 - 何も出力しない（深度のみ書き込まれる）
void main(PSInput input)
{
    // 深度値のみを書き込み、カラー出力は不要
    // リターン値を持たない場合、深度バッファのみに値が書き込まれる
}