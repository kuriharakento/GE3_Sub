#pragma once
#include "math/VectorFunc.h"

/**
 * 点光源
 */
struct PointLight
{
	Vector4 color;				// ライトの色
	Vector3 position;			// ライトの位置
	float intensity;			// ライトの強さ
	float radius;				// ライトの届く最大距離
	float decay;				// ライトの減衰率
};