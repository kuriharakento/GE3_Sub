#pragma once
#include <string>

#include "MatrixFunc.h"
#include "Vector3.h"

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};

// Quaternionの積
Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

// Quaternionのスカラー乗算演算子の定義
Quaternion operator*(const Quaternion& q, float scalar);

// Quaternionのスカラー乗算演算子の定義（逆順）
Quaternion operator*(float scalar, const Quaternion& q);

// Quaternionの加算演算子の定義
Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);

// Quaternionの積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

// 単位Quaternionの作成を返す
Quaternion IdentityQuaternion();

// 共役Quaternionを返す
Quaternion Conjugate(const Quaternion& quaternion);

// Quaternionのnormを返す
float Norm(const Quaternion& quaternion);

// 正規化したQuaternionを返す
Quaternion Normalize(const Quaternion& quaternion);

//逆Quaternionを返す
Quaternion Inverse(const Quaternion& quaternion);
// 任意軸回転を表すQuaternionの生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

// Quaternionから回転行列を求める
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

// Quaternionを画面に表示する関数
void QuaternionScreenPrintf(int x, int y, const Quaternion& quaternion, const std::string& title);

// 球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
