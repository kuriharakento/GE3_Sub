#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>

#include "GraphicsTypes.h"

//スプライト共通部分のポインタ
class Object3dCommon;

class Object3d
{
public:	//メンバ関数
	/**
	 * \brief 初期化
	 */
	void Initialize(Object3dCommon* object3dCommon);

	/**
	 * \brief 更新
	 */
	void Update();

	/**
	 * \brief 描画
	 * \param commandList 
	 */
	void Draw();

	/**
	 * \brief 行列の更新
	 */
	void UpdateMatrix();

	/**
	 * \brief .mtlファイルの読み取り
	 * \param directoryPath 
	 * \param filename 
	 * \return 
	 */
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/**
	 * \brief .objファイルの読み取り
	 * \param a 
	 * \param a 
	 * \return a
	 */
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

public: //ゲッター



private: //メンバ関数
	/**
	 * \brief 頂点データの生成
	 */
	void CreateVertexData();

	/**
	 * \brief マテリアルデータの生成
	 */
	void CreateMaterialData();

	/**
	 * \brief 座標変換行列の生成
	 */
	void CreateWvpData();

	/**
	 * \brief 平行光源データの生成
	 */
	void CreateDirectionalLightData();

	/**
	 * \brief 描画設定の初期化
	 */
	void InitializeRenderingSettings();

private: //描画用変数
	//オブジェクトのコマンド
	Object3dCommon* object3dCommon_ = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;

	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransformationMatrix* transformationMatrixData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
public:
	Transform transform_;
private: //メンバ変数
	//Objファイルのデータ
	ModelData modelData_;

	//座標変換行列
	
	Transform cameraTransform_;

};

