#pragma once
#include "math/VectorFunc.h"

/**
 * スポットライト
 */
struct GPUSpotLight
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

struct CPUSpotLight {
    GPUSpotLight gpuData;

    // 補間用のメンバを追加
    Vector4 startColor;
    Vector4 endColor;
    float duration;       // 補間にかける時間
    float elapsedTime;    // 経過時間
    bool isReversing;     // 補間の方向
    bool isGradientActive; // グラデーションが有効かどうか
    std::function<float(float)> easingFunction; // イージング関数
};