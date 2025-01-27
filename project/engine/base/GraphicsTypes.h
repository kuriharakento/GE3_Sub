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
	Vector4 position;							// 位置
	Vector2 texcoord;							// テクスチャ座標
	Vector3 normal;								// 法線
};

/**
 * \brief マテリアル
 */
struct Material
{
	Vector4 color;								// 色
	int32_t enableLighting;						// ライティングの有効無効
	float padding[3];							// 12バイト（アラインメント用）
	Matrix4x4 uvTransform;						// UV変換行列
	float shininess;							// 反射強度
	float padding2[3];							// 12バイト（アラインメント用）
};

/**
 * \brief 座標変換行列データ
 */
struct TransformationMatrix
{
	Matrix4x4 WVP;								// ワールドビュープロジェクション行列
	Matrix4x4 World;							// ワールド行列
	Matrix4x4 WorldInverseTranspose;			// ワールド逆転置行列
};

//トランスフォーム
struct Transform
{
	Vector3 scale;								// スケール
	Vector3 rotate;								// 回転
	Vector3 translate;							// 平行移動
};

/**
 * \brief マテリアルデータ
 */
struct MaterialData
{
	std::string textureFilePath;				// テクスチャファイルパス
	uint32_t textureIndex = 0;					// テクスチャインデックス
};

/**
 * \brief モデルデータ
 */
struct ModelData
{
	std::vector<VertexData> vertices;			// 頂点データ
	MaterialData material;						// マテリアルデータ
};

/**
 * \brief ライト
 */
struct DirectionalLight
{
	Vector4 color;								// ライトの色
	Vector3 direction;							// ライトの向き
	float intensity;							// ライトの強さ
};

/**
 * \brief パーティクル
 */
struct Particle
{
	Transform transform;						// トランスフォーム
	Vector3 velocity;							// 速度
	Vector4 color;								// 色
	float lifeTime;								// 寿命
	float currentTime;							// 現在の時間
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
	Vector3 worldPos;							// ワールド座標
	float padding;								// 4バイト（アラインメント用）
};

//点光源
struct PointLight
{
	Vector4 color;								// ライトの色
	Vector3 position;							// ライトの位置
	float intensity;							// ライトの強さ
	float radius;								// ライトの届く最大距離
	float decay;								// ライトの減衰率
};