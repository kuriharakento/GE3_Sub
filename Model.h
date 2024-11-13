#pragma once
#include "ModelCommon.h"
#include "GraphicsTypes.h"

class Model
{
public:

	/**
	 * \brief 初期化
	 * \param modelCommon 
	 */
	void Initialize(ModelCommon* modelCommon);

	/**
	 * \brief 描画
	 */
	void Draw();

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
	 * \brief 描画設定の初期化
	 */
	void InitializeRenderingSettings();

private:
	//モデルコマンド
	ModelCommon* modelCommon_;

	//Objファイルのデータ
	ModelData modelData_;

	/*-----------------------[ 頂点 ]------------------------*/

	//バッファのリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//データ
	VertexData* vertexData_ = nullptr;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	/*-----------------------[ マテリアル ]------------------------*/

	//バッファのリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//データ
	Material* materialData_ = nullptr;

};

