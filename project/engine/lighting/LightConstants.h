#pragma once
#include <cstdint>

namespace LightMaxCount
{
	//ディレクショナルライトの最大数
	//constexpr uint32_t kMaxDirectionalLightCount = 10;
	//ポイントライトの最大数
	constexpr uint32_t kMaxPointLightCount = 10;
	//スポットライトの最大数
	constexpr uint32_t kMaxSpotLightCount = 10;
}

struct LightCount
{
	//ディレクショナルライトの数
	//uint32_t directionalLightCount = 0;
	//ポイントライトの数
	uint32_t pointLightCount = 0;
	//スポットライトの数
	uint32_t spotLightCount = 0;
	//アライメント
	uint32_t padding;
};
