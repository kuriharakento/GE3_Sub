#include "3d/Object3d.h"

#include "3d/Object3dCommon.h"
#include "math/MathUtils.h"
#include "lighting/LightManager.h"
#include "lighting/ShadowMapManager.h"

///////////////////////////////////////////////////////////////////////
///						>>>基本的な処理<<<							///
///////////////////////////////////////////////////////////////////////

Object3d::~Object3d()
{
	//座標変換行列のリソースを解放
	if (wvpResource_)
	{
		wvpResource_->Unmap(0, nullptr);
		wvpResource_.Reset();
	}
	//平行光源のリソースを解放
	if (directionalLightResource_)
	{
		directionalLightResource_->Unmap(0, nullptr);
		directionalLightResource_.Reset();
	}
	//カメラのリソースを解放
	if (cameraResource_)
	{
		cameraResource_->Unmap(0, nullptr);
		cameraResource_.Reset();
	}
	//ディレクショナルライトのリソースを解放
	if (directionalLightResource_)
	{
		directionalLightResource_->Unmap(0, nullptr);
		directionalLightResource_.Reset();
	}
	
}

void Object3d::Initialize(Object3dCommon* object3dCommon,Camera* camera)
{
	//引数で受け取った物を記録する
	object3dCommon_ = object3dCommon;
	//引数が指定されていれば引数のカメラを使う。指定されていなければデフォルトのカメラを使う
	camera_ = camera ? camera : object3dCommon->GetDefaultCamera();

	//描画設定の初期化
	InitializeRenderingSettings();

	//Transformの変数を作る
	transform_ = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f },
	};

}

void Object3d::Update(CameraManager* camera)
{
	camera_ = camera ? camera->GetActiveCamera() : camera_;
	//座標変換行列の更新
	UpdateMatrix(camera_);
}

void Object3d::Draw()
{
	//座標変換行列CBufferの場所を設定
	object3dCommon_->GetDXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//平行光源CBufferの場所を設定
	object3dCommon_->GetDXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	//カメラCBufferの場所を設定
	object3dCommon_->GetDXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());

	//ライトマネージャーがあればライトの描画を行う
	if (lightManager_)
	{
		lightManager_->Draw();
	}

	//3Dモデルが割り当てられていれば描画する
	if(model_)
	{
		model_->Draw();
	}
}

///////////////////////////////////////////////////////////////////////
///						>>>その他関数の処理<<<							///
///////////////////////////////////////////////////////////////////////

void Object3d::UpdateMatrix(Camera* camera)
{
	//引数が指定されていれば引数のカメラを使う。指定されていなければデフォルトのカメラを使う
	camera_ = camera ? camera : object3dCommon_->GetDefaultCamera();

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix;
	Matrix4x4 worldInverseTransposeMatrix = MathUtils::Transpose(Inverse(worldMatrix));

	if(camera)
	{
		const Matrix4x4& viewProjectionMatrix = camera->GetViewProjectionMatrix();
		worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
		cameraData_->worldPos = { camera->GetWorldMatrix().m[3][0],camera->GetWorldMatrix().m[3][1],camera->GetWorldMatrix().m[3][2]};
	} else {
		worldViewProjectionMatrix = worldMatrix;
	}

	transformationMatrixData_->WVP = model_->GetModelData().rootNode.localMatrix * worldViewProjectionMatrix;
	transformationMatrixData_->World = model_->GetModelData().rootNode.localMatrix * worldMatrix;
	transformationMatrixData_->WorldInverseTranspose = worldInverseTransposeMatrix;
}

void Object3d::DrawForShadow(const DirectX::XMMATRIX& lightViewProjection)
{
	// シャドウキャストしない場合は何もしない
	if (!castShadow_)
	{
		return; 
	}

	//　ワールド行列
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	// ライト視点でのワールドビュープロジェクション行列を計算
	Matrix4x4 shadowWVP = model_->GetModelData().rootNode.localMatrix * worldMatrix * lightViewProjection;

	// シャドウマップ用のワールド行列をセット（一時的に上書き）
	Matrix4x4 originalWVP = transformationMatrixData_->WVP;
	transformationMatrixData_->WVP = shadowWVP;

	// 定数バッファをシャドウマップパイプラインのスロットにセット
	object3dCommon_->GetDXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(
		0, // シャドウマップパイプラインでのレジスタ番号
		wvpResource_->GetGPUVirtualAddress()
	);

	// モデルの頂点/インデックスバッファをセットして描画
	ID3D12GraphicsCommandList* cmdList = object3dCommon_->GetDXCommon()->GetCommandList();

	// 頂点バッファとインデックスバッファの設定
	cmdList->IASetVertexBuffers(0, 1, &model_->GetVertexBufferView());
	
	// 描画コマンドの発行
	cmdList->DrawIndexedInstanced(
		static_cast<UINT>(model_->GetModelData().vertices.size()),
		1, 0, 0, 0
	);

	// 元のWVP行列に戻す
	transformationMatrixData_->WVP = originalWVP;
}

void Object3d::AddShadowMap(uint32_t srvIndex, const Matrix4x4& lightViewProj, ShadowMapType type, const std::string& lightName, float bias)
{
	if (activeShadowMapCount_ >= MAX_SHADOW_MAPS) {
        Logger::Log("Maximum shadow maps reached for object");
        return;
    }
    
    auto& entry = shadowMaps_[activeShadowMapCount_];
    entry.srvIndex = srvIndex;
    entry.enabled = true;
    entry.type = type;
    entry.lightName = lightName;
    
    // 定数バッファを作成（まだなければ）
    if (!entry.constantBuffer) {
        entry.constantBuffer = object3dCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(ShadowMapConstants));
        entry.constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&entry.constantData));
    }
    
    // シャドウマップデータを更新
    entry.constantData->lightViewProjection = lightViewProj;
    entry.constantData->shadowBias = bias;
    entry.constantData->shadowMapSize = static_cast<float>(ShadowMapData::kShadowMapSize);
    
    activeShadowMapCount_++;
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
	directionalLightData_->direction = Vector3::Normalize({ 0.0f,-1.0f,0.0f });
	directionalLightData_->intensity = 1.0f;
}

void Object3d::CreateCameraData()
{
	/*--------------[ カメラリソースを作る ]-----------------*/

	cameraResource_ = object3dCommon_->GetDXCommon()->CreateBufferResource(sizeof(CameraForGPU));

	/*--------------[ カメラリソースにデータを書き込むためのアドレスを取得してcameraDataに割り当てる ]-----------------*/

	cameraResource_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(&cameraData_)
	);
  
	//デフォルト値は以下のようにしておく
	cameraData_->worldPos = {};
}


void Object3d::InitializeRenderingSettings()
{
	//座標変換行列の生成
	CreateWvpData();

	//平行光源データの生成
	CreateDirectionalLightData();

	//カメラデータの生成
	CreateCameraData();
}

void Object3d::CreateShadowMapData()
{

}
