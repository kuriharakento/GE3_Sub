#include "Object3d.h"

#include <cassert>

#include "Object3dCommon.h"
#include "TextureManager.h"

///////////////////////////////////////////////////////////////////////
///						>>>基本的な処理<<<							///
///////////////////////////////////////////////////////////////////////

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
	//引数で受け取った物を記録する
	object3dCommon_ = object3dCommon;

	//描画設定の初期化
	InitializeRenderingSettings();

	//Transformの変数を作る
	transform_ = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f },
	};

	cameraTransform_ = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,4.0f,-10.0f },
	};

}

void Object3d::Update()
{
	//座標変換行列の更新
	UpdateMatrix();
}

void Object3d::Draw()
{
	//座標変換行列CBufferの場所を設定
	object3dCommon_->GetDXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//平行光源CBufferの場所を設定

	//3Dモデルが割り当てられていれば描画する
	if(model_)
	{
		model_->Draw();
	}
}

///////////////////////////////////////////////////////////////////////
///						>>>その他関数の処理<<<							///
///////////////////////////////////////////////////////////////////////

void Object3d::UpdateMatrix()
{
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	transformationMatrixData_->WVP = worldMatrix * viewMatrix * projectionMatrix;
	transformationMatrixData_->World = worldMatrix;
}

void Object3d::CreateWvpData()
{
	/*--------------[ 座標変換行列リソースを作る ]-----------------*/

	wvpResource_ = object3dCommon_->GetDXCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	/*--------------[ 座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる ]-----------------*/

	wvpResource_->Map(0,
		nullptr,
		reinterpret_cast<void**>(&transformationMatrixData_)
	);

	//単位行列を書き込んでおく
	transformationMatrixData_->WVP = MakeIdentity4x4();
	transformationMatrixData_->World = MakeIdentity4x4();

}

void Object3d::CreateDirectionalLightData()
{
	/*--------------[ 平行光源リソースを作る ]-----------------*/

	directionalLightResource_ = object3dCommon_->GetDXCommon()->CreateBufferResource(sizeof(DirectionalLight));

	/*--------------[ 平行光源リソースにデータを書き込むためのアドレスを取得してdirectionalLightDataに割り当てる ]-----------------*/

	directionalLightResource_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(&directionalLightData_)
	);

	//デフォルト値は以下のようにしておく
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = Normalize({ 0.0f,-1.0f,0.0f });
	directionalLightData_->intensity = 1.0f;

}

void Object3d::InitializeRenderingSettings()
{
	//座標変換行列の生成
	CreateWvpData();

	//平行光源データの生成
	CreateDirectionalLightData();
}
