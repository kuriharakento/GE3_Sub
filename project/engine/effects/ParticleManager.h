#pragma once
#include <random>
#include <wrl.h>
#include <d3d12.h>
#include <list>
#include <unordered_map>

#include "ParticlePipelineManager.h"
#include "engine/manager/CameraManager.h"
#include "base/GraphicsTypes.h"
#include "3d/Model.h"

//前方宣言
class DirectXCommon;
class SrvManager;

//パーティクルの最大数
constexpr uint32_t kMaxParticleCount = 100;

enum class VertexShape
{
	Plane,
	Ring
};

struct ParticleGroup
{
	MaterialData materialData;
	std::list<Particle> particles;
	uint32_t instancingSrvIndex = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = nullptr;
	uint32_t instanceCount = 0;
	ParticleForGPU* instancingData = nullptr;
	//モデルの頂点データ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	VertexData* vertexData = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	bool isBillboard = true; // ビルボードフラグ
	//マテリアルデータ
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	Material* materialData_ = nullptr;
};

class ParticleManager
{
public:
	//シングルトンのインスタンスを取得
	static ParticleManager* GetInstance();
	//シングルトンの解放
	static void Finalize();
	///初期化
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
	///更新
	void Update(CameraManager* camera);
	///描画
	void Draw();
	/**
	 * \brief パーティクルグループの作成
	 * \param groupName グループ名
	 * \param textureFilePath テクスチャファイルパス
	 */
	void CreateParticleGroup(const std::string& groupName, const std::string& textureFilePath);
	/**
	 * \brief パーティクルの生成
	 * \param groupName グループ名
	 * \param position 位置
	 * \param count 生成するパーティクルの数
	 */
	void Emit(const std::string& groupName, const Vector3& position, uint32_t count);
	/**
	 * \brief 平面用のパーティクルの生成
	 * \param groupName グループ名
	 * \param position 位置
	 * \param count 生成するパーティクルの数
	 */
	void EmitPlane(const std::string& groupName, const Vector3& position, uint32_t count);

	void EmitRing(const std::string& groupName, const Vector3& position, uint32_t count);

	void EmitCylinder(const std::string& groupName, const Vector3& position, uint32_t count);

	/**
	 * \brief パーティクルのテクスチャの設定
	 * \param groupName グループ名
	 * \param textureFilePath テクスチャファイルパス
	 */
	void SetTexture(const std::string& groupName, const std::string& textureFilePath);
	/**
	* \brief パーティクルの回転の設定
	* \param groupName グループ名
	* \param rotation 回転ベクトル
	*/
	void SetRotate(const std::string& groupName, const Vector3& rotation);
	/**
	 * \brief ランダムな回転の設定
	 * \param groupName グループ名
	 * \param rotation 回転ベクトル
	 */
	void SetRandomRotate(const std::string& groupName);
	/**
	 * \brief スケールの設定
	 * \param groupName グループ名
	 * \param scale スケールベクトル
	 */
	void SetScale(const std::string& groupName, const Vector3& scale);
	/**
	 * \brief ランダムなスケールの設定
	 * \param groupName グループ名
	 */
	void SetRandomScale(const std::string& groupName);
	/**
	 * \brief 速度ベクトルの設定
	 * \param groupName グループ名
	 * \param velocity 速度ベクトル
	 */
	void SetVelocity(const std::string& groupName, const Vector3& velocity);

	void SetVertexData(const std::string& groupName, VertexShape shape);

	void SetBillboard(const std::string& groupName, bool isBillboard) { particleGroups_[groupName].isBillboard = isBillboard; }

private: //メンバ関数
	/**
	 * \brief モデルデータの初期化
	 */
	void InitializeModelData();
	/**
	* \brief パーティクルの生成
	* \param position 位置
	* \return 生成したパーティクル
	*/
	Particle MakeNewParticle(const Vector3& position);
	/**
	 * \brief 平面用のパーティクルの生成
	 * \param position 位置
	 * \return 生成したパーティクル
	 */
	Particle MakeNewPlaneParticle(const Vector3& position);

private: //メンバ変数
	/*--------------[ ポインタ ]-----------------*/

	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	Model* model_ = nullptr;
	//パイプラインマネージャー
	std::unique_ptr<ParticlePipelineManager> pipelineManager_ = nullptr;

	//ランダムエンジン
	std::mt19937 mt_;

	/*--------------[ マテリアルデータ ]-----------------*/

	
	//モデルデータ
	ModelData modelData_;

	/*--------------[ コンテナ ]-----------------*/

	//グループ名をキーとしてパーティクルグループを管理
	std::unordered_map<std::string, ParticleGroup> particleGroups_;

private:
	/*========[ シングルトン ]========*/
	static ParticleManager* instance_;
	//コピー禁止
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager& rhs) = delete;
	ParticleManager& operator=(const ParticleManager& rhs) = delete;
};

