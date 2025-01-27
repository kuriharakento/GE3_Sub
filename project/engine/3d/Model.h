#pragma once
#include <d3d12.h>
#include <string>
#include <wrl.h>

#include "ModelCommon.h"
#include "base/GraphicsTypes.h"

class Model
{
public:

	/**
	 * \brief 初期化
	 * \param modelCommon 
	 */
	void Initialize(ModelCommon* modelCommon,const std::string& directoryPath, const std::string& filename);

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

	//static MaterialData LoadMaterialTemplateFile(const std::string& filePath);

	/**
	 * \brief .objファイルの読み取り
	 * \param
	 * \param
	 * \return
	 */
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	/**
	 * \brief .objファイルの読み取り
	 * \param filePath 
	 * \return 
	 */
	//static ModelData LoadObjFile(const std::string& filePath);

public: //アクセッサ
	Vector4 GetColor() const { return materialData_->color; }
	void SetColor(const Vector4& color) { materialData_->color = color; }

	//ライティングの有効無効
	bool IsEnableLighting() const { return materialData_->enableLighting; }
	void SetEnableLighting(bool enable) { materialData_->enableLighting = enable; }

	//反射強度
	void SetShininess(float shininess) { materialData_->shininess = shininess; }
	float GetShininess() const { return materialData_->shininess; }

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

