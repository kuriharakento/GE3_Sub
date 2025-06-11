#pragma once
#include <array>
#include <cstdint>
#include <d3d12.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <wrl.h>

#include "ShadowMapPipeline.h"

class SrvManager;
class DirectXCommon;

enum class ShadowMapType
{
    SpotLight,      // 単一のシャドウマップ
    PointLight,     // キューブマップ（6面）
    DirectionalLight // カスケードシャドウマップ
};

struct ShadowMapData
{
    Microsoft::WRL::ComPtr<ID3D12Resource> shadowMapTexture;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissorRect;
    uint32_t srvIndex = 0;
    ShadowMapType type;

    static const uint32_t kShadowMapSize = 1024;
    static const uint32_t kCascadeCount = 4; // CSM用
};

// PointLight用キューブマップ
struct CubeShadowMapData
{
    std::array<ShadowMapData, 6> faces; // 6面分
    ShadowMapType type = ShadowMapType::PointLight;
};

// DirectionalLight用カスケードシャドウマップ
struct CascadeShadowMapData
{
    std::array<ShadowMapData, ShadowMapData::kCascadeCount> cascades; // 4段階
    std::array<float, ShadowMapData::kCascadeCount> cascadeDistances; // 各カスケードの距離
    ShadowMapType type = ShadowMapType::DirectionalLight;
};


class ShadowMapManager
{
public:
    static ShadowMapManager* GetInstance();

    void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
    void Finalize();

    // 各ライトタイプ用のシャドウマップ作成
    void CreateSpotLightShadowMap(const std::string& name, uint32_t size = ShadowMapData::kShadowMapSize);
    void CreatePointLightShadowMap(const std::string& name, uint32_t size = ShadowMapData::kShadowMapSize);
    void CreateDirectionalLightShadowMap(const std::string& name, uint32_t size = ShadowMapData::kShadowMapSize);

    // シャドウマップ描画の開始/終了
    void RenderAllShadowMaps();

    void RenderAllSpotLightShadowMap();
	void RenderAllPointLightShadowMaps();
	void RenderAllDirectionalLightShadowMaps();

    // シャドウマップの取得
    uint32_t GetSpotLightSRVIndex(const std::string& name);
    std::array<uint32_t, 6> GetPointLightSRVIndices(const std::string& name); // 6面分のSRVインデックス
    std::array<uint32_t, 4> GetDirectionalLightSRVIndices(const std::string& name); // 4カスケード分

    // シャドウマップの削除
    void RemoveShadowMap(const std::string& name);
    void ClearAllShadowMaps();

    // ライトタイプの取得
    ShadowMapType GetShadowMapType(const std::string& name);

    // デバッグ用
    bool HasShadowMap(const std::string& name) const;
    void LogAllShadowMaps() const;

private:
    ShadowMapManager() = default;
    ~ShadowMapManager() = default;
    ShadowMapManager(const ShadowMapManager&) = delete;
    ShadowMapManager& operator=(const ShadowMapManager&) = delete;

private:
    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;
	std::unique_ptr<ShadowMapPipeline> shadowMapPipeline_;

    // 各タイプのシャドウマップ
    std::unordered_map<std::string, std::unique_ptr<ShadowMapData>> spotLightShadowMaps_;
    std::unordered_map<std::string, std::unique_ptr<CubeShadowMapData>> pointLightShadowMaps_;
    std::unordered_map<std::string, std::unique_ptr<CascadeShadowMapData>> directionalLightShadowMaps_;

    static ShadowMapManager* instance_;
};

