#pragma once
#include <array>
#include <cstdint>
#include <d3d12.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <wrl.h>

#include "ShadowMapPipeline.h"
#include "math/MatrixFunc.h"

class LightManager;
class SceneManager;
class BaseScene;
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

	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, SceneManager* sceneManager, LightManager* lightManager);
    void Finalize();

    // シャドウマップ生成
    void CreateSpotLightShadowMap(const std::string& name);
	void CreatePointLightShadowMap(const std::string& name);
	void CreateDirectionalLightShadowMap(const std::string& name, const std::array<float, ShadowMapData::kCascadeCount>& cascadeDistances);

    // シャドウマップ描画開始
    void BeginSpotLightShadowPass(const std::string& name);
    void BeginPointLightShadowPass(const std::string& name, uint32_t faceIndex);
    void BeginDirectionalLightShadowPass(const std::string& name, uint32_t cascadeIndex);
    void EndShadowPass();

    // シャドウマップ描画
    void RenderAllShadowMaps();

    // シャドウマップ用のビュープロジェクション行列計算
    Matrix4x4 CalculateSpotLightViewProjection(const std::string& name);
    Matrix4x4 CalculatePointLightViewProjection(const std::string& name, uint32_t faceIndex);
    Matrix4x4 CalculateDirectionalLightViewProjection(const std::string& name, uint32_t cascadeIndex);

    // シャドウマップSRVの取得
    uint32_t GetSpotLightShadowMapSrvIndex(const std::string& name) const;
    uint32_t GetPointLightShadowMapSrvIndex(const std::string& name, uint32_t faceIndex = 0) const;
    uint32_t GetDirectionalLightShadowMapSrvIndex(const std::string& name, uint32_t cascadeIndex = 0) const;

    // シャドウマップの削除
    void RemoveSpotLightShadowMap(const std::string& name);
    void RemovePointLightShadowMap(const std::string& name);
    void RemoveDirectionalLightShadowMap(const std::string& name);

private:
    ShadowMapManager() = default;
    ~ShadowMapManager() = default;
    ShadowMapManager(const ShadowMapManager&) = delete;
    ShadowMapManager& operator=(const ShadowMapManager&) = delete;

	// シャドウマップリソースを生成
	ShadowMapData CreateShadowMapResource();

private:
    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;
	SceneManager* sceneManager_ = nullptr;
	LightManager* lightManager_ = nullptr;
	std::unique_ptr<ShadowMapPipeline> shadowMapPipeline_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> shadowSamplerHeap_; // シャドウマップ用サンプラーヒープ
	D3D12_CPU_DESCRIPTOR_HANDLE shadowSamplerHandle_; // サンプラーハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE shadowSamplerGpuHandle_; // GPU用サンプラーハンドル

    // 各タイプのシャドウマップ
    std::unordered_map<std::string, std::unique_ptr<ShadowMapData>> spotLightShadowMaps_;
    std::unordered_map<std::string, std::unique_ptr<CubeShadowMapData>> pointLightShadowMaps_;
    std::unordered_map<std::string, std::unique_ptr<CascadeShadowMapData>> directionalLightShadowMaps_;

    static ShadowMapManager* instance_;
};

