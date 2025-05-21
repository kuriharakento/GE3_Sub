#pragma once
#include "IPostEffect.h"
#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d12.h>
#include <string>

#include "GrayscaleEffect.h"

class DirectXCommon;
class SrvManager;

class PostProcessManager
{
public:
    PostProcessManager();
    ~PostProcessManager();

    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, const std::wstring& vsPath, const std::wstring& psPath);
    void Draw(D3D12_GPU_DESCRIPTOR_HANDLE inputTexture);

	std::unique_ptr<GrayscaleEffect> grayscaleEffect_;

private:
    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    void SetupPipeline(const std::wstring& vsPath, const std::wstring& psPath);
};
