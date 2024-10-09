#include "SpriteCommon.h"

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	//引数で受け取ってメンバ変数に記録する
	dxCommon_ = dxCommon;

	//グラフィックスパイプラインの生成
	CreateGraphicsPipelineState();
}

void SpriteCommon::CommonRenderingSetting()
{
	//ルートシグネチャをセットするコマンド
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	//グラフィックスパイプラインステートをセットするコマンド
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());

	//プリミティブトポロジーをセットするコマンド
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void SpriteCommon::CreateRootSignature()
{
}

void SpriteCommon::CreateGraphicsPipelineState()
{
}
