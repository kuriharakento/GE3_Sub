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
	Vector4 color;			// 16バイト
	int32_t enableLighting;	// 4バイト
	float padding[3];		// 12バイト（アラインメント用）
	Matrix4x4 uvTransform;	// 64バイト
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

/**
 * \brief パーティクル
 */
struct Particle
{
	Transform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};
struct ParticleForGPU
{
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

//カメラ
struct CameraForGPU
{
	Vector3 worldPos;
	float padding;
};