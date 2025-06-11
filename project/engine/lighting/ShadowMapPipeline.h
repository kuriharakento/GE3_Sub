#pragma once
#include <wrl.h>
#include <d3d12.h>

class DirectXCommon;

class ShadowMapPipeline
{
public:
	void Initialize(DirectXCommon* dxCommon);


private:
	void CreateRootSignature();
	void CreatePipelineState();

private:
	DirectXCommon* dxCommon_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
};

