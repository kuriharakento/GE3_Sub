#pragma once
#include "math/VectorFunc.h"

/**
 * スポットライト
 */
struct SpotLight
{
	Vector4 color;					// ライトの色
	Vector3 position;				// ライトの位置
	float intensity;				// ライトの強さ
	Vector3 direction;				// ライトの向き
	float distance;					// ライトの届く最大距離
	float decay;					// ライトの減衰率
	float cosAngle;					// ライトの余弦
	float cosFalloffStart;			// フォールオフ開始角度の余弦
};