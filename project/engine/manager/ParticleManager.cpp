#include "ParticleManager.h"

#include <dxcapi.h>

#include "TextureManager.h"
#include "3d/ModelManager.h"
#include "base/DirectXCommon.h"
#include "base/Logger.h"
#include "manager/SrvManager.h"

ParticleManager* ParticleManager::instance_ = nullptr;

ParticleManager* ParticleManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ParticleManager();
	}
	return instance_;
}

void ParticleManager::Finalize()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	//引数をメンバ変数に記録
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	//ランダムエンジンの初期化
	std::random_device rd;
	mt_.seed(rd());

	//パーティクルグループの初期化
	particleGroups_.clear();

	//頂点データの初期化
	InitializeVertexData();

	//パイプラインの生成
	CreateGraphicsPipelineState();
}

void ParticleManager::Update(Camera* camera)
{
	uint32_t numInstance = 0;
	const float kDeltaTime = 1.0f / 60.0f;

	// particleGroups での処理
	for (auto& groupPair : particleGroups_) {
		ParticleGroup& group = groupPair.second;

		// 各グループ内のパーティクルを処理
		for (std::list<Particle>::iterator particleItr = group.particles.begin(); particleItr != group.particles.end();)
		{
			if (particleItr->lifeTime <= particleItr->currentTime)
			{
				particleItr = group.particles.erase(particleItr);
				continue;
			}

			// パーティクルの位置更新
			particleItr->transform.translate += particleItr->velocity * kDeltaTime;
			particleItr->currentTime += kDeltaTime;

			// パーティクルの透明度計算
			float alpha = 1.0f - (particleItr->currentTime / particleItr->lifeTime);

			// インスタンス数の制限を守る
			if (numInstance < kMaxParticleCount)
			{
				// ビルボード用の行列計算
				Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(0.0f);
				Matrix4x4 billboardMatrix = MakeIdentity4x4();

				// カメラの回転を適用
				billboardMatrix = Multiply(backToFrontMatrix, camera->GetWorldMatrix());

				billboardMatrix.m[3][0] = 0.0f;
				billboardMatrix.m[3][1] = 0.0f;
				billboardMatrix.m[3][2] = 0.0f;

				// スケール行列と移動行列の生成
				Matrix4x4 scaleMatrix = MakeScaleMatrix(particleItr->transform.scale);
				Matrix4x4 translateMatrix = MakeTranslateMatrix(particleItr->transform.translate);

				// 世界行列とWVP行列の計算
				Matrix4x4 worldMatrixInstancing = scaleMatrix * billboardMatrix * translateMatrix;
				Matrix4x4 worldViewProjectionMatrixInstancing = Multiply(worldMatrixInstancing, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));

				// インスタンシングデータの設定
				instancingData[numInstance].WVP = worldViewProjectionMatrixInstancing;
				instancingData[numInstance].World = worldMatrixInstancing;
				instancingData[numInstance].color = particleItr->color;
				instancingData[numInstance].color.w = alpha;

				// インスタンスの数をインクリメント
				++numInstance;
			}

			// 次のパーティクルに進める
			++particleItr;
		}
	}

}

void ParticleManager::Draw()
{
	/*--------------[ ルートシグネチャの設定 ]-----------------*/

	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	/*--------------[ パイプラインステートの設定 ]-----------------*/

	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());

	/*--------------[ プリミティブトポロジーの設定 ]-----------------*/

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*--------------[ VBVを設定 ]-----------------*/

	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	for (auto& groupPair : particleGroups_)
	{
		ParticleGroup& group = groupPair.second;
		// SRVのDescriptorTableの先頭を設定
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(group.materialData->textureIndex));
		// インスタンシングデータのSRVのDescriptorTableの先頭を設定
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(group.instanceIndex));
		// 描画
		dxCommon_->GetCommandList()->DrawInstanced(6, group.instanceCount, 0, 0);
	}

}

void ParticleManager::CreateParticleGroup(const std::string& groupName, const std::string& textureFilePath)
{
	if (particleGroups_.find(groupName) != particleGroups_.end())
	{
		Logger::Log("Particle group with name " + groupName + " already exists.");
		assert(false);
	}

	//パーティクルグループを作成し、コンテナに追加
	ParticleGroup newGroup;

	newGroup.materialData = new MaterialData();

	ModelData modelData_;
	modelData_ = Model::LoadObjFile("Resources", "axis.obj");
	modelData_.material.textureFilePath = std::string("./") + modelData_.material.textureFilePath;
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
	//VertexBufferViewを設定する
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();				//リソースの先頭のアドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//マテリアルデータにテクスチャのインデックスを設定
	newGroup.materialData->textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);
	//インスタンシング用のリソースを作成
	newGroup.instancingResource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kMaxParticleCount);

	// デバッグ情報を出力
	if (newGroup.instancingResource == nullptr) {
		Logger::Log("Failed to create instancing resource");
	} else {
		Logger::Log("Instancing resource created successfully");
	}

	newGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&newGroup.instancingData));
	
	//インスタンシング用にSRVを確保してSRVインデックスを記録
	srvManager_->Allocate();
	//SRV生成（StructuredBuffer用の設定）
	srvManager_->CreateSRVforStructuredBuffer(
		newGroup.materialData->textureIndex,
		newGroup.instancingResource.Get(),
		kMaxParticleCount, // numElements: パーティクルの最大数
		sizeof(ParticleForGPU) // structureByteStride: 各パーティクルのサイズ
	);


	// 新しいパーティクルグループを追加
	particleGroups_[groupName] = newGroup;
}

void ParticleManager::Emit(const std::string& groupName, const Vector3& position, uint32_t count)
{
	//パーティクルグループが存在しない場合は作成
	if (particleGroups_.find(groupName) == particleGroups_.end())
	{
		CreateParticleGroup(groupName, std::string("a"));
	}

	//パーティクルを生成
	particleGroups_[groupName].instanceCount = count;
}

void ParticleManager::CreateRootSignature()
{
	///===================================================================
	///ディスクリプタレンジの生成
	///===================================================================

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;	//０から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1;		//数は１つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	//SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	///===================================================================
	///RootSignatureを生成する
	///===================================================================

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成。複数設定できるので配列。今回は結果１つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;						//レジスタ番号０とバインド

	/*rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;*/

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;			//descriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;					//vertexShader
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;			//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;					//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);		//Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.RegisterSpace = 0;
	rootParameters[3].Descriptor.ShaderRegister = 1;

	//Smaplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;				//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;			//0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;			//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;							//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;									//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		//PixelShaderを使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	descriptionRootSignature.pParameters = rootParameters;					//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		//配列の長さ

	//シリアライズしてバイナリする
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリをもとに生成
	hr = dxCommon_->GetDevice()->CreateRootSignature(
		0, 
		signatureBlob->GetBufferPointer(), 
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void ParticleManager::InitializeVertexData()
{
	/*--------------[ VertexResourceを作る ]-----------------*/

	vertexResource_ = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kMaxParticleCount);//１頂点当たりのサイズ

	/*--------------[ VertexBufferViewを設定する ]-----------------*/

	ModelData modelData_;
	modelData_ = Model::LoadObjFile("Resources", "axis.obj");
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();				//リソースの先頭のアドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);									//１頂点当たりのサイズ

	/*--------------[ VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる ]-----------------*/

	vertexResource_->Map(0,
		nullptr,
		reinterpret_cast<void**>(&instancingData)
	);

	for (uint32_t i = 0; i < kMaxParticleCount; ++i)
	{
		instancingData[i].WVP = MakeIdentity4x4();
		instancingData[i].World = MakeIdentity4x4();
		instancingData[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

}

void ParticleManager::CreateGraphicsPipelineState()
{
	//ルートシグネチャ
	CreateRootSignature();

	HRESULT hr;

	///===================================================================
	///InputLayout(インプットレイアウト)
	///===================================================================

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	///===================================================================
	///BlendState(ブレンドステート)
	///===================================================================

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	///===================================================================
	///RasterizerState(ラスタライザステート)
	///===================================================================

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	///===================================================================
	///ShaderをCompileする
	///===================================================================

	//shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileSharder(L"Resources/shaders/Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon_->CompileSharder(L"Resources/shaders/Particle.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	///===================================================================
	///DepthStencilStateの設定を行う
	///===================================================================

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	///===================================================================
	///PSOを生成する
	///===================================================================

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定（気にしなくていい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//実際に生成

	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc, 
		IID_PPV_ARGS(&graphicsPipelineState_)
	);

	assert(SUCCEEDED(hr));
}
