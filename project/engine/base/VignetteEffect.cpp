#include "VignetteEffect.h"
#include "DirectXTex/d3dx12.h"
#include "engine/base/DirectXCommon.h"
#include "engine/manager/SrvManager.h"
#include "ImGui/imgui.h"

VignetteEffect::VignetteEffect() = default;
VignetteEffect::~VignetteEffect() = default;

void VignetteEffect::ImGuiUpdate()
{
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Vignette Effect"))
	{
		isDirty_ |= ImGui::Checkbox("Enabled", &enabled_);
		isDirty_ |= ImGui::SliderFloat("Intensity", &params_.intensity, 0.0f, 1.0f);
		isDirty_ |= ImGui::SliderFloat("Radius", &params_.radius, 0.0f, 1.0f);
		isDirty_ |= ImGui::SliderFloat("Softness", &params_.softness, 0.01f, 1.0f);
		isDirty_ |= ImGui::ColorEdit3("Color", &params_.color.x);
	}
#endif
}

void VignetteEffect::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, PostProcessManager* postProcessManager)
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

void VignetteEffect::Draw()
{
	auto cmdList = dxCommon_->GetCommandList();
	//定数バッファの更新
	UpdateParameters();
	//// ルートパラメータセット（パイプラインのルートレイアウトに合わせて）
	//cmdList->SetComputeRootDescriptorTable(0, inputSRV);  // 入力テクスチャSRV
	//cmdList->SetComputeRootDescriptorTable(1, outputUAV); // 出力テクスチャUAV
	cmdList->SetComputeRootConstantBufferView(2, constantBuffer_->GetGPUVirtualAddress());
	// Dispatchサイズは画面サイズに合わせる（例）
	/*UINT dispatchX = (UINT)ceilf((float)WinApp::kClientWidth / 16);
	UINT dispatchY = (UINT)ceilf((float)WinApp::kClientHeight / 16);
	cmdList->Dispatch(dispatchX, dispatchY, 1);*/
}


void VignetteEffect::CopyDataToConstantBuffer(void* mappedData)
{
	// 定数バッファにパラメータをコピー
	Parameters* param = reinterpret_cast<Parameters*>(mappedData);
	param->intensity = params_.intensity;
	param->radius = params_.radius;
	param->softness = params_.softness;
	param->color = params_.color;
	param->enabled = enabled_ ? 1 : 0; // 有効フラグをセット
	isDirty_ = false; // パラメータが変更されたことを示す
}

void VignetteEffect::SetIntensity(const float& intensity)
{
	if (params_.intensity != intensity)
	{
		params_.intensity = intensity;
		isDirty_ = true; // パラメータが変更されたことを示す
	}
}

void VignetteEffect::SetRadius(const float& radius)
{
	if (params_.radius != radius)
	{
		params_.radius = radius;
		isDirty_ = true; // パラメータが変更されたことを示す
	}
}

void VignetteEffect::SetSoftness(const float& softness)
{
	if (params_.softness != softness)
	{
		params_.softness = softness;
		isDirty_ = true; // パラメータが変更されたことを示す
	}
}

void VignetteEffect::SetColor(const Vector3& color)
{
	if (params_.color != color)
	{
		params_.color = color;
		isDirty_ = true; // パラメータが変更されたことを示す
	}
}
