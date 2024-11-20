#pragma once
#include <string>
#include <vector>

#include "math/MatrixFunc.h"
#include "math/VectorFunc.h"

/**
 * \brief 頂点データ
 */
struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

/**
 * \brief マテリアル
 */
struct Material
{
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

/**
 * \brief 座標変換行列データ
 */
struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};

//トランスフォーム
struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

/**
 * \brief マテリアルデータ
 */
struct MaterialData
{
	std::string textureFilePath;
	uint32_t textureIndex = 0;
};

/**
 * \brief モデルデータ
 */
struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
};

/**
 * \brief ライト
 */
struct DirectionalLight
{
	Vector4 color;
	Vector3 direction;
	float intensity;
};
