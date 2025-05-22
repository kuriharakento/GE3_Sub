#include "BasePostEffect.h"
#include "DirectXTex/d3dx12.h"
#include <cassert>

BasePostEffect::BasePostEffect() : enabled_(false), isDirty_(true) {}

void BasePostEffect::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, PostProcessManager* postProcessManager)
{
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;
	postProcessManager_ = postProcessManager;
}


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