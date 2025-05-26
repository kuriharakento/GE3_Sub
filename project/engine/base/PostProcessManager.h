#pragma once
#include "IPostEffect.h"
#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d12.h>
#include <string>

#include "FullScreenEffect.h"
#include "GrayscaleEffect.h"
#include "VignetteEffect.h"

class DirectXCommon;
class SrvManager;

class PostProcessManager
{
public:
    PostProcessManager();
    ~PostProcessManager();

    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
    void Draw(D3D12_GPU_DESCRIPTOR_HANDLE inputTexture);
	void ImGuiUpdate();

	//========== ポストエフェクト ==========//

	// リスト
	std::vector<std::unique_ptr<BasePostEffect>> postEffects_;

	//========== フルスクリーンエフェクト(エフェクトの適用) ==========//
	// NOTE: フルスクリーンエフェクトは、ポストプロセスの最後に適用される
	std::unique_ptr<FullScreenEffect> fullScreenEffect_;

private:
    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;
    // パイプライン
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState_;
    // 中間Ping-Pongバッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> tempTextureA_;
    // SRVハンドル（GPUハンドル）
    D3D12_GPU_DESCRIPTOR_HANDLE tempSRVA_;
    // UAVハンドル（GPUハンドル） ← 追加
    D3D12_GPU_DESCRIPTOR_HANDLE tempUAVA_;
    // 状態管理
    D3D12_RESOURCE_STATES tempTextureAState_ = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

    void CreateIntermediateResources();
    void CreateComComputePipelineState(const std::wstring& csPath);
};
