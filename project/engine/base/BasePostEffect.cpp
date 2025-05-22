#include "BasePostEffect.h"
#include "DirectXTex/d3dx12.h"
#include <cassert>

BasePostEffect::BasePostEffect() : enabled_(false), isDirty_(true) {}

//void BasePostEffect::Initialize(DirectXCommon* dxCommon)
//{
//    dxCommon_ = dxCommon;
//
//    // 定数バッファの作成
//    size_t bufferSize = GetConstantBufferSize();
//    bufferSize = (bufferSize + 255) & ~255; // 256バイトアラインメント
//
//    D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
//    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
//
//    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
//        &heapProps,
//        D3D12_HEAP_FLAG_NONE,
//        &bufferDesc,
//        D3D12_RESOURCE_STATE_GENERIC_READ,
//        nullptr,
//        IID_PPV_ARGS(&constantBuffer_));
//    assert(SUCCEEDED(hr));
//
//    // 初期データの更新
//    UpdateParameters();
//
//	// サンプラーヒープの作成
//	dxCommon_->CreateSamplerHeap();
//}

void BasePostEffect::UpdateParameters()
{
    if (!isDirty_) return;

    void* mappedData = nullptr;
    constantBuffer_->Map(0, nullptr, &mappedData);
    CopyDataToConstantBuffer(mappedData);
    constantBuffer_->Unmap(0, nullptr);

    isDirty_ = false;
}

void BasePostEffect::SetEnabled(bool enabled)
{
	if (enabled_ != enabled)
	{
        enabled_ = enabled;
		isDirty_ = true; // パラメータが変更されたことを示す
	}
}