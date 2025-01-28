#pragma once
#include <unordered_map>
#include <string>

#include "LightConstants.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "base/DirectXCommon.h"
#include "externals/imgui/imgui.h"

class LightManager
{
public:
	//コンストラクタ
	LightManager();

	~LightManager();

	//初期化
	void Initialize(DirectXCommon* dxCommon);

	//更新
	void Update();

	//描画
	void Draw();

	//ポイントライトの追加
	void AddPointLight(const std::string& name);

	// スポットライトの追加
	void AddSpotLight(const std::string& name);

	//ライトの削除
	void Clear();

public: //アクセッサ
	//ポイントライトの取得
	const PointLight& GetPointLight(const std::string& name) const;
	//スポットライトの取得
	const SpotLight& GetSpotLight(const std::string& name) const;

	//ポイントライトの数の取得
	const uint32_t& GetPointLightCount() const;
	//スポットライトの数の取得
	const uint32_t& GetSpotLightCount() const;

private:
	//ImGui
	void ImGui();

	//定数バッファの作成
	void CreateConstantBuffer();

private:
	//ポイントライト
	std::unordered_map<std::string, PointLight> pointLights_;
	//ポイントライトの名前
	std::vector<std::string> pointLightNames_;

	//スポットライト
	std::unordered_map<std::string, SpotLight> spotLights_;
	//スポットライトの名前
	std::vector<std::string> spotLightNames_;

	//ライトの数
	LightCount lightCount_;

	//DxCommon
	DirectXCommon* dxCommon_ = nullptr;

	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> lightCountResource_;

	//書き込むデータ
	PointLight* pointLightData_ = nullptr;
	SpotLight* spotLightData_ = nullptr;
	LightCount* lightCountData_ = nullptr;
};
