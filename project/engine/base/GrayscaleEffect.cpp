#include "GrayscaleEffect.h"
#include <cassert>

#include "base/DirectXCommon.h"
#include "DirectXTex/d3dx12.h"
#include "ImGui/imgui.h"
#include "manager/SrvManager.h"

GrayscaleEffect::GrayscaleEffect()
{
    // デフォルトパラメータの設定
    params_.intensity = 1.0f;
	params_.enabled = 1;
}

GrayscaleEffect::~GrayscaleEffect()
{
    
}

void GrayscaleEffect::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager,
	PostProcessManager* postProcessManager)
{
	BasePostEffect::Initialize(dxCommon, srvManager, postProcessManager);

	// 定数バッファの作成
	D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(Parameters) + 255) & ~255); // 256バイトアラインメント
	HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBuffer_));
	assert(SUCCEEDED(hr));

	//初期データの書き込み
	isDirty_ = true;  // パラメータが変更されたことを示す
	UpdateParameters();
}

void GrayscaleEffect::Draw()
{
	auto cmdList = dxCommon_->GetCommandList();
	//定数バッファの更新
	UpdateParameters();

    //// ルートパラメータセット（パイプラインのルートレイアウトに合わせて）
    //cmdList->SetComputeRootDescriptorTable(0, inputSRV);  // 入力テクスチャSRV
    //cmdList->SetComputeRootDescriptorTable(1, outputUAV); // 出力テクスチャUAV
    cmdList->SetComputeRootConstantBufferView(3, constantBuffer_->GetGPUVirtualAddress());

    // Dispatchサイズは画面サイズに合わせる（例）
    /*UINT dispatchX = (UINT)ceilf((float)WinApp::kClientWidth / 16);
    UINT dispatchY = (UINT)ceilf((float)WinApp::kClientHeight / 16);

    cmdList->Dispatch(dispatchX, dispatchY, 1);*/
}

void GrayscaleEffect::ImGuiUpdate()
{
#ifdef _DEBUG
	// ImGuiで強度を調整するスライダーを表示
	if (ImGui::CollapsingHeader("Grayscale Effect"))
	{
		isDirty_ |= ImGui::Checkbox("Enabled", &enabled_);
		isDirty_ |= ImGui::SliderFloat("Grayscale Intensity", &params_.intensity, 0.0f, 1.0f);
	}
#endif
}


void GrayscaleEffect::SetIntensity(float intensity)
{
    // 値が変更された場合のみ更新フラグを立てる
    if (params_.intensity != intensity)
    {
        params_.intensity = intensity;
        isDirty_ = true;  // 基底クラスのフラグを使用
    }
}

void GrayscaleEffect::CopyDataToConstantBuffer(void* mappedData)
{
    // 定数バッファにデータをコピー
    Parameters* data = static_cast<Parameters*>(mappedData);
    data->intensity = params_.intensity;
    data->enabled = enabled_ ? 1 : 0;  // 基底クラスのenabledフラグを使用
}