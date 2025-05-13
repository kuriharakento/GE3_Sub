#pragma once
#include "Vector3.h"

struct AABB
{
	Vector3 min; // 最小点
	Vector3 max; // 最大点
	AABB() : min(Vector3()), max(Vector3()) {}
	AABB(const Vector3& min, const Vector3& max) : min(min), max(max) {}
	// AABBの中心を取得
	Vector3 GetCenter() const {
		return (min + max) * 0.5f;
	}
	// AABBのサイズを取得
	Vector3 GetSize() const {
		return max - min;
	}
};
