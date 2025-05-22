#pragma once
#include "BasePostEffect.h"

class FullScreenEffect : public BasePostEffect {
public:
    FullScreenEffect();
    ~FullScreenEffect() override;

    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, const std::wstring& vsPath, const std::wstring& psPath) override;
    void Draw(D3D12_GPU_DESCRIPTOR_HANDLE inputSRV, D3D12_CPU_DESCRIPTOR_HANDLE outputRTV) override;
    void CopyDataToConstantBuffer(void* mappedData) override {};
    
};
