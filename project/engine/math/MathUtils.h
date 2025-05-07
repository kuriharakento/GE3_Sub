#pragma once
#include "MatrixFunc.h"

namespace MathUtils
{
	///座標変換
	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

	///法線ベクトルの変換
	Vector3 TransformNormal(const Vector3& normal, const Matrix4x4& matrix);

	///円軌道の計算
	Vector3 CalculateOrbitPosition(const Vector3& center, float radius, float angle);

	//回転角度の計算関数
	Vector3 CalculateYawPitchFromDirection(const Vector3& direction);

	// 現在位置とターゲット位置から向きを計算
	Vector3 CalculateDirectionToTarget(const Vector3& currentPosition, const Vector3& targetPosition);

	//転置行列
	Matrix4x4 Transpose(const Matrix4x4& m);

};