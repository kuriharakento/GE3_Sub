#pragma once
#include "math/VectorFunc.h"

#undef min
#undef max

struct AABB
{
	Vector3 min;	//最小座標
	Vector3 max;	//最大座標
	Vector3 pos;	//中心座標

	//コンストラクタ
	AABB() : min(Vector3()), max(Vector3()),pos(Vector3()){}

	//交差判定
	bool Intersects(const AABB& other) const
	{
		//自身の最大座標が他の最小座標より小さい、または自身の最小座標が他の最大座標より大きい場合は交差していない
		return (min.x <= other.max.x && max.x >= other.min.x) &&
			(min.y <= other.max.y && max.y >= other.min.y) &&
			(min.z <= other.max.z && max.z >= other.min.z);
	}
};
