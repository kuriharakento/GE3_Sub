#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>

#include "base/GraphicsTypes.h"
#include "3d/Model.h"
#include "3d/ModelManager.h"
#include "manager/CameraManager.h"

//スプライト共通部分のポインタ
class Object3dCommon;

class Object3d
{
public:	/*========[ メンバ関数 ]========*/
	~Object3d();
	/**
	 * \brief 初期化
	 */
	void Initialize(Object3dCommon* object3dCommon,Camera* camera = nullptr);

	/**
	 * \brief 更新
	 */
	void Update(CameraManager* camera = nullptr);

	/**
	 * \brief 描画
	 * \param commandList 
	 */
	void Draw();

	/**
	 * \brief 行列の更新
	 */
	void UpdateMatrix(Camera* camera);

public: /*========[ ゲッター ]========*/
	//Transform
	const Vector3& GetScale() const { return transform_.scale; }
	const Vector3& GetRotate() const { return transform_.rotate; }
	const Vector3& GetTranslate() const { return transform_.translate; }

	//色
	Vector4 GetColor() const { return model_->GetColor(); }

public: /*========[ セッター ]========*/

	//モデルの設定
	void SetModel(Model* model) { model_ = model; }
	void SetModel(const std::string& filePath) { model_ = ModelManager::GetInstance()->FindModel(filePath); }

	//カメラの設定
	void SetCamera(Camera* camera) { camera_ = camera; }

	//Transform
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

	//色
	void SetColor(const Vector4& color) { model_->SetColor(color); }

private: /*========[ プライベートメンバ関数(このクラス内でしか使わない関数)  ]========*/

	/**
	 * \brief 座標変換行列の生成
	 */
	void CreateWvpData();

	/**
	 * \brief 平行光源データの生成
	 */
	void CreateDirectionalLightData();

	//カメラデータの生成
	void CreateCameraData();

	/**
	 * \brief 描画設定の初期化
	 */
	void InitializeRenderingSettings();
	

private: /*========[ 描画用変数 ]========*/
	//オブジェクトのコマンド
	Object3dCommon* object3dCommon_ = nullptr;

	//バッファリソース	
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;

	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;
	CameraForGPU* cameraData_ = nullptr;

private: /*========[ メンバ変数 ]========*/
	//カメラ
	Camera* camera_ = nullptr;

	//モデル
	Model* model_ = nullptr;

	//座標変換行列
	Transform transform_;

};

