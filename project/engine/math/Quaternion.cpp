#include "Quaternion.h"
#include <cmath>

// Quaternionの乗算演算子の定義
Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
	Quaternion result;
	result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
	result.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;
	result.z = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;
	return result;
}

// Quaternionのスカラー乗算演算子の定義
Quaternion operator*(const Quaternion& q, float scalar)
{
	return Quaternion{ q.x * scalar, q.y * scalar, q.z * scalar, q.w * scalar };
}

// Quaternionのスカラー乗算演算子の定義（逆順）
Quaternion operator*(float scalar, const Quaternion& q)
{
	return q * scalar;
}

// Quaternionの加算演算子の定義
Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
{
	return Quaternion{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	Quaternion result;
	result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
	result.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;
	result.z = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;
	return result;
}

Quaternion IdentityQuaternion()
{
	return Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
}

Quaternion Conjugate(const Quaternion& quaternion)
{
	return Quaternion{ -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
}

float Norm(const Quaternion& quaternion)
{
	return std::sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
}

Quaternion Normalize(const Quaternion& quaternion)
{
	float norm = Norm(quaternion);
	return Quaternion{ quaternion.x / norm, quaternion.y / norm, quaternion.z / norm, quaternion.w / norm };
}

Quaternion Inverse(const Quaternion& quaternion)
{
	float norm = Norm(quaternion);
	Quaternion conjugate = Conjugate(quaternion);
	return Quaternion{ conjugate.x / (norm * norm), conjugate.y / (norm * norm), conjugate.z / (norm * norm), conjugate.w / (norm * norm) };
}

// 任意軸回転を表すQuaternionの生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	Vector3 normalizedAxis = axis.Normalize();
	float halfAngle = angle * 0.5f;
	float sinHalfAngle = std::sin(halfAngle);

	return Quaternion{
		normalizedAxis.x * sinHalfAngle,
		normalizedAxis.y * sinHalfAngle,
		normalizedAxis.z * sinHalfAngle,
		std::cos(halfAngle)
	};
}

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
	Quaternion qVector = { 0, vector.x, vector.y, vector.z };
	Quaternion qConjugate = { quaternion.w, -quaternion.x, -quaternion.y, -quaternion.z };

	Quaternion result = quaternion * qVector * qConjugate;

	return { result.x, result.y, result.z };
}

// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion)
{
	// クォータニオンを正規化
	Quaternion q = Normalize(quaternion);

	Matrix4x4 matrix;

	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;

	// X軸とY軸の数値を交換
	matrix.m[0][0] = 1.0f - 2.0f * (yy + zz);
	matrix.m[0][1] = 2.0f * (xy + wz);
	matrix.m[0][2] = 2.0f * (xz - wy);
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = 2.0f * (xy - wz);
	matrix.m[1][1] = 1.0f - 2.0f * (xx + zz);
	matrix.m[1][2] = 2.0f * (yz + wx);
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 2.0f * (xz + wy);
	matrix.m[2][1] = 2.0f * (yz - wx);
	matrix.m[2][2] = 1.0f - 2.0f * (xx + yy);
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = 0.0f;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	Quaternion q1Copy = q1;
	float dot = q0.x * q1Copy.x + q0.y * q1Copy.y + q0.z * q1Copy.z + q0.w * q1Copy.w;
	if (dot < 0.0f)
	{
		q1Copy = Quaternion{ -q1Copy.x, -q1Copy.y, -q1Copy.z, -q1Copy.w };
		dot = -dot;
	}

	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);
	float s0 = std::sin((1.0f - t) * theta) / sinTheta;
	float s1 = std::sin(t * theta) / sinTheta;
	return q0 * s0 + q1Copy * s1;
}