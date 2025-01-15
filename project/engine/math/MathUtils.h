#pragma once

#include "VectorFunc.h"
#include "MatrixFunc.h"

class MathUtils
{
public:
	///座標変換
	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
};

