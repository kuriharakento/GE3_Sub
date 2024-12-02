#pragma once
#include "DirectXCommon.h"

class SrvManager
{
public:
	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//確保
	uint32_t Allocate();

	//SRV生成（テクスチャ用）
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format,UINT mipLevels);
	//SRV生成（Structured Buffer用）
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	void PreDraw();

	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

public: //アクセッサ

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

private:
	DirectXCommon* dxCommon_ = nullptr;

	//最大SRV数
	static const uint32_t kMaxSRVCount;
	//SRVのディスクリプタサイズ
	uint32_t descriptorSize_;
	//SRVのディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	//次に使用するSRVインデックス
	uint32_t useIndex_ = 0;


};

