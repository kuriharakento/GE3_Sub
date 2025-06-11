#include "ShadowMapManager.h"

#include "base/Logger.h"
#include "base/DirectXCommon.h"
#include  "manager/SrvManager.h"

ShadowMapManager* ShadowMapManager::instance_ = nullptr;

ShadowMapManager* ShadowMapManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ShadowMapManager();
	}
	return instance_;
}


void ShadowMapManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;
	// シャドウマップパイプラインの初期化
	shadowMapPipeline_ = std::make_unique<ShadowMapPipeline>();
	shadowMapPipeline_->Initialize(dxCommon);

	Logger::Log("ShadowMapManager initialized");
}

void ShadowMapManager::Finalize()
{
	ClearAllShadowMaps();
	delete instance_;
	instance_ = nullptr;

	Logger::Log("ShadowMapManager finalized");
}

void ShadowMapManager::RenderAllShadowMaps()
{
	// 全てのスポットライトシャドウマップをレンダリング
	RenderAllSpotLightShadowMap();

	// 全てのポイントライトシャドウマップをレンダリング
	RenderAllPointLightShadowMaps();

	// 全てのディレクショナルライトシャドウマップをレンダリング
	RenderAllDirectionalLightShadowMaps();
}

void ShadowMapManager::RenderAllSpotLightShadowMap()
{
	for (auto& pair : spotLightShadowMaps_)
	{
		pair.second->RenderShadowMap(dxCommon_, srvManager_, shadowMapPipeline_.get());
	}
}


