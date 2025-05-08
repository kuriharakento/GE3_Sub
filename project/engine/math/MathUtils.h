#pragma once
#include <cassert>
#include <numbers>

#include "MatrixFunc.h"

namespace MathUtils
{
	// 線形補間（Lerp）関数
	static float Lerp(float start, float end, float t)
	{
		return start + (end - start) * t;
	}
	static Vector3 Lerp(const Vector3& start, const Vector3& end, float t)
	{
		return start + (end - start) * t;
	}

	///座標変換
	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
	{
		Vector3 result;
		result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
		result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
		result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
		float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
		assert(w != 0.0f);
		result.x /= w;
		result.y /= w;
		result.z /= w;
		return result;
	}

	// 角度を -π ～ π の範囲に正規化
	static float NormalizeAngleRad(float angle)
	{
		while (angle > std::numbers::pi_v<float>) angle -= 2.0f * std::numbers::pi_v<float>;
		while (angle < -std::numbers::pi_v<float>) angle += 2.0f * std::numbers::pi_v<float>;
		return angle;
	}

	// 単一軸の角度補間（最短経路）
	static float LerpAngle(float start, float end, float t)
	{
		float delta = NormalizeAngleRad(end - start);
		return start + delta * t;
	}
	static Vector3 LerpAngle(const Vector3& from, const Vector3& to, float t) {
		return {
			LerpAngle(from.x, to.x, t),
			LerpAngle(from.y, to.y, t),
			LerpAngle(from.z, to.z, t)
		};
	}

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