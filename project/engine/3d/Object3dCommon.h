#pragma once
#include "base/DirectXCommon.h"

class Object3dCommon
{
public: //メンバ関数
	/// \brief 初期化
	void Initialize(DirectXCommon* dxCommon);

	//共通描画設定
	void CommonRenderingSetting();

public: //アクセッサ
	DirectXCommon* GetDXCommon() const { return dxCommon_; }

private: //メンバ関数
	/// \brief ルートシグネチャの生成
	void CreateRootSignature();
	/// \brief グラフィックスパイプラインステートの生成
	void CreateGraphicsPipelineState();

private: //メンバ変数
	//DirectXコマンド
	DirectXCommon* dxCommon_ = nullptr;

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	//グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;


};
