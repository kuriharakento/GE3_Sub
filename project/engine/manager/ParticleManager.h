#pragma once
#include <random>
#include <wrl.h>
#include <d3d12.h>
#include <list>
#include <unordered_map>

#include "base/Camera.h"
#include "base/GraphicsTypes.h"

//前方宣言
class DirectXCommon;
class SrvManager;

//パーティクルの最大数
constexpr uint32_t kMaxParticleCount = 100;

struct ParticleGroup
{
	MaterialData* materialData = nullptr;
	std::list<Particle> particles;
	uint32_t instanceIndex = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
	uint32_t instanceCount = 0;
	ParticleForGPU* instancingData = nullptr;
};

class ParticleManager
{
public:
	/**
	 * \brief 初期化
	 * \param dxCommon 
	 * \param srvManager 
	 */
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);

	void Update(Camera* camera);

	void Draw();

	void CreateParticleGroup(const std::string& groupName, const std::string& textureFilePath);

private: /*========[ メンバ関数 ]========*/

	/**
	 * \brief パイプラインの生成
	 */
	void CreateGraphicsPipelineState();

	/**
	 * \brief ルートシグネチャの生成
	 */
	void CreateRootSignature();

	/**
	 * \brief 頂点データの初期化
	 */
	void InitializeVertexData();

private: /*========[ メンバ変数 ]========*/

	/*--------------[ ポインタ ]-----------------*/

	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	//パイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	//ランダムエンジン
	std::mt19937 mt_;

	/*--------------[ 頂点データ ]-----------------*/

	//バッファのリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//データ
	ParticleForGPU* instancingData = nullptr;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	/*--------------[ コンテナ ]-----------------*/

	//グループ名をキーとしてパーティクルグループを管理
	std::unordered_map<std::string, ParticleGroup> particleGroups_;

};

