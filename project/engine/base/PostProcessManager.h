#pragma once
#include "IPostEffect.h"
#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d12.h>
#include <string>

#include "FullScreenEffect.h"
#include "GrayscaleEffect.h"

class DirectXCommon;
class SrvManager;

class PostProcessManager
{
public:
    PostProcessManager();
    ~PostProcessManager();

    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
    void Draw(D3D12_GPU_DESCRIPTOR_HANDLE inputTexture);

	std::unique_ptr<GrayscaleEffect> grayscaleEffect_;
	std::unique_ptr<FullScreenEffect> fullScreenEffect_;

private:
    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;

    // 中間Ping-Pongバッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> tempTextureA_;
    Microsoft::WRL::ComPtr<ID3D12Resource> tempTextureB_;
    D3D12_GPU_DESCRIPTOR_HANDLE tempSRVA_;
    D3D12_GPU_DESCRIPTOR_HANDLE tempSRVB_;
    D3D12_CPU_DESCRIPTOR_HANDLE tempRTVA_;
    D3D12_CPU_DESCRIPTOR_HANDLE tempRTVB_;

    D3D12_RESOURCE_STATES tempTextureAState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;

    void CreateIntermediateResources();
};
