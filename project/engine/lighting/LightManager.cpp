#include "LightManager.h"

#include <numbers>

#include "base/Logger.h"

LightManager::LightManager()
{
	//ライトの数を初期化
	lightCount_.pointLightCount = 0;
	lightCount_.spotLightCount = 0;
}

LightManager::~LightManager()
{
	if (lightCountResource_) lightCountResource_->Release();
	if (pointLightResource_) pointLightResource_->Release();
	if (spotLightResource_) spotLightResource_->Release();
}

void LightManager::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;
	//定数バッファの作成
	CreateConstantBuffer();

	//ポイントライトの追加
	AddPointLight("pointLight" + std::to_string(pointLights_.size()));

	//スポットライトの追加
	AddSpotLight("spotLight" + std::to_string(spotLights_.size()));
}

void LightManager::Update()
{
	//ImGuiの表示
	ImGui();

	// ポイントライトのデータを更新
	uint32_t pointLightIndex = 0;
	for (const auto& [name, light] : pointLights_)
	{
		pointLightData_[pointLightIndex++] = light;
	}

	// スポットライトのデータを更新
	uint32_t spotLightIndex = 0;
	for (const auto& [name, light] : spotLights_)
	{
		spotLightData_[spotLightIndex++] = light;
	}

	lightCount_.pointLightCount = pointLightIndex;
	lightCount_.spotLightCount = spotLightIndex;

	// 定数バッファにライトの数を更新
	lightCountData_->pointLightCount = lightCount_.pointLightCount;
	lightCountData_->spotLightCount = lightCount_.spotLightCount;
}

void LightManager::Draw()
{
	//ポイントライトのCBVを設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	//スポットライトのCBVを設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());
	//ライトの数のCBVを設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, lightCountResource_->GetGPUVirtualAddress());
}

void LightManager::AddPointLight(const std::string& name)
{
	//最大個数に達している場合は追加しない
	if (pointLights_.size() >= LightMaxCount::kMaxPointLightCount)
	{
		Logger::Log("ポイントライトの最大数に達しているため追加できません\n");
		return;
	}
	
	//ポイントライトを作成と初期化
	PointLight pointLight;
	pointLight.color = { 1.0f,1.0f,1.0f,1.0f };
	pointLight.position = { 0.0f,2.0f,0.0f };
	pointLight.intensity = 1.0f;
	pointLight.radius = 3.0f;
	pointLight.decay = 1.0f;
	pointLights_.emplace(name, pointLight);
	//名前を保存
	pointLightNames_.push_back(name);
	//ライトの数をインクリメント
	++lightCount_.pointLightCount;
}

void LightManager::AddSpotLight(const std::string& name)
{
	//最大個数に達している場合は追加しない
	if (spotLights_.size() >= LightMaxCount::kMaxSpotLightCount)
	{
		Logger::Log("スポットライトの最大数に達しているため追加できません\n");
		return;
	}

	//スポットライトを作成と初期化
	SpotLight spotLight;
	spotLight.color = { 1.0f,1.0f,1.0f,1.0f };
	spotLight.position = { 0.0f,1.0f,0.0f };
	spotLight.distance = 7.0f;
	spotLight.intensity = 4.0f;
	spotLight.direction = Vector3::Normalize({ 0.0f,-1.0f,1.0f });
	spotLight.cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLight.decay = 2.0f;
	spotLight.cosFalloffStart = 1.0f;
	spotLights_.emplace(name, spotLight);
	//名前を保存
	spotLightNames_.push_back(name);
	//ライトの数をインクリメント
	++lightCount_.spotLightCount;
}

void LightManager::Clear()
{
	pointLights_.clear();
}

const PointLight& LightManager::GetPointLight(const std::string& name) const
{
	return pointLights_.at(name);
}

const SpotLight& LightManager::GetSpotLight(const std::string& name) const
{
	return spotLights_.at(name);
}

const uint32_t& LightManager::GetPointLightCount() const
{
	return lightCount_.pointLightCount;
}

const uint32_t& LightManager::GetSpotLightCount() const
{
	return lightCount_.spotLightCount;
}

void LightManager::CreateConstantBuffer()
{
	/*--------------[ ライトの数リソースを作る ]-----------------*/

	lightCountResource_ = dxCommon_->CreateBufferResource(sizeof(LightCount));

	/*--------------[ ライトの数リソースにデータを書き込むためのアドレスを取得してlightCountDataに割り当てる ]-----------------*/

	lightCountResource_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(&lightCountData_)
	);

	//デフォルト値は以下のようにしておく
	lightCountData_->pointLightCount = 0;
	lightCountData_->spotLightCount = 0;

	/*--------------[ ポイントライトリソースを作る ]-----------------*/

	pointLightResource_ = dxCommon_->CreateBufferResource(sizeof(PointLight) * LightMaxCount::kMaxPointLightCount);

	/*--------------[ ポイントライトリソースにデータを書き込むためのアドレスを取得してpointLightDataに割り当てる ]-----------------*/

	pointLightResource_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(&pointLightData_)
	);

	//デフォルト値は以下のようにしておく
	pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData_->position = { 0.0f,0.0f,0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 3.0f;

	/*--------------[ スポットライトリソースを作る ]-----------------*/

	spotLightResource_ = dxCommon_->CreateBufferResource(sizeof(SpotLight) * LightMaxCount::kMaxSpotLightCount);

	/*--------------[ スポットライトリソースにデータを書き込むためのアドレスを取得してspotLightDataに割り当てる ]-----------------*/

	spotLightResource_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(&spotLightData_)
	);

	//デフォルト値は以下のようにしておく
	spotLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData_->position = { 0.0f,0.0f,0.0f };
	spotLightData_->intensity = 1.0f;
	spotLightData_->distance = 3.0f;
	spotLightData_->decay = 1.0f;
	spotLightData_->direction = { 0.0f,-1.0f,0.0f };
	spotLightData_->cosAngle = 0.0f;
	spotLightData_->cosFalloffStart = 0.0f;
}

void LightManager::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("LightManager");
	ImGui::Text("PointLight Count :%d", lightCount_.pointLightCount);
	ImGui::Text("SpotLight Count :%d", lightCount_.spotLightCount);
	if (ImGui::Button("clear"))
	{
		Clear();
	}
	//ライトの追加
	if (ImGui::Button("Add PointLight"))
	{
		AddPointLight("PointLight" + std::to_string(pointLights_.size()));
	}
	ImGui::SameLine();
	if (ImGui::Button("Add SpotLight"))
	{
		AddSpotLight("SpotLight" + std::to_string(spotLights_.size()));
	}
	//ポイントライトの設定
	for (const auto& name : pointLightNames_)
	{
		ImGui::PushID(name.c_str());
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			ImGui::ColorEdit4("PointLight Color", &pointLights_.at(name).color.x);
			ImGui::DragFloat3("PointLight Position", &pointLights_.at(name).position.x, 0.1f);
			ImGui::DragFloat("PointLight Intensity", &pointLights_.at(name).intensity, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("PointLight Radius", &pointLights_.at(name).radius, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("PointLight Decay", &pointLights_.at(name).decay, 0.1f, 0.0f, 5.0f);
		}
		ImGui::PopID();
	}
	//スポットライトの設定
	for (const auto& name : spotLightNames_)
	{
		ImGui::PushID(name.c_str());
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			ImGui::ColorEdit4("SpotLight Color", &spotLights_.at(name).color.x);
			ImGui::DragFloat3("SpotLight Position", &spotLights_.at(name).position.x, 0.1f);
			ImGui::DragFloat3("SpotLight Direction", &spotLights_.at(name).direction.x, 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat("SpotLight Intensity", &spotLights_.at(name).intensity, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("SpotLight Distance", &spotLights_.at(name).distance, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("SpotLight CosAngle", &spotLights_.at(name).cosAngle, 0.01f, -3.14f, 3.14f);
			ImGui::DragFloat("SpotLight Decay", &spotLights_.at(name).decay, 0.1f, 0.0f, 5.0f);
			ImGui::DragFloat("SpotLight CosFalloffStart", &spotLights_.at(name).cosFalloffStart, 0.01f, -3.14f, 3.14f);
		}
		ImGui::PopID();
	}
	ImGui::End();
#endif
}

