#pragma once

#include <map>
#include <string>
#include <memory>

#include "Model.h"
#include "ModelCommon.h"

class ModelManager
{
public: /*========[ メンバ関数 ]========*/
	//シングルトンのインスタンスを取得
	static ModelManager* GetInstance();

	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//終了
	void Finalize();


	void LoadModel(const std::string& filePath);

private: /*========[ シングルトン ]========*/
	static ModelManager* instance_;
	
	//コピー禁止
	ModelManager()=default;
	~ModelManager()=default;
	
	ModelManager(const ModelManager& rhs) = delete;
	ModelManager& operator=(const ModelManager& rhs) = delete;

private: /*========[ メンバ変数 ]========*/
	ModelCommon* modelCommon_ = nullptr;

	//モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models_;
};
