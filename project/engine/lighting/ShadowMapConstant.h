#pragma once
#include "math/MatrixFunc.h"

struct ShadowMapConstants
{
    Matrix4x4 lightViewProjection;
    float shadowBias;
    float shadowMapSize;
    float padding[2];
};

enum class ShadowMapType
{
	SpotLight,      // 単一のシャドウマップ
	PointLight,     // キューブマップ（6面）
	DirectionalLight // カスケードシャドウマップ
};