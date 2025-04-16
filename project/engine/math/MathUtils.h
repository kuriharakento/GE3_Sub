#pragma once
#include "MatrixFunc.h"

namespace MathUtils
{
	///座標変換
	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

	//転置行列
	Matrix4x4 Transpose(const Matrix4x4& m);

};