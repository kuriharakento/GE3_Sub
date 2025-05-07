#include "MathUtils.h"


namespace MathUtils
{
	Vector3 TransformNormal(const Vector3& normal, const Matrix4x4& matrix)
	{
		Vector3 result;
		result.x = normal.x * matrix.m[0][0] + normal.y * matrix.m[1][0] + normal.z * matrix.m[2][0];
		result.y = normal.x * matrix.m[0][1] + normal.y * matrix.m[1][1] + normal.z * matrix.m[2][1];
		result.z = normal.x * matrix.m[0][2] + normal.y * matrix.m[1][2] + normal.z * matrix.m[2][2];
		return result;
	}

	Vector3 CalculateOrbitPosition(const Vector3& center, float radius, float angle)
	{
		return center + Vector3(std::cos(angle) * radius, 0.0f, std::sin(angle) * radius);
	}

	Vector3 CalculateYawPitchFromDirection(const Vector3& direction)
	{
		if(direction.LengthSquared() == 0.0f)
		{
			//回転不要
			return Vector3(0.0f, 0.0f, 0.0f);
		}

		direction.Normalize();

		float yaw = std::atan2(direction.x, direction.z);
		float pitch = std::atan2(direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z));
		return Vector3(-pitch, yaw, 0.0f);
	}

	Vector3 CalculateDirectionToTarget(const Vector3& currentPosition, const Vector3& targetPosition)
	{
		// ターゲット方向のベクトルを計算
		Vector3 direction = targetPosition - currentPosition;

		// ベクトルの長さが0の場合、回転不要
		if (direction.IsZero())
		{
			return Vector3(0.0f, 0.0f, 0.0f);
		}

		// ベクトルを正規化
		direction = direction.Normalize();

		// Yaw（左右の回転角度）を計算
		float yaw = std::atan2(direction.x, direction.z);

		// Pitch（上下の回転角度）を計算
		float horizontalDistance = std::sqrt(direction.x * direction.x + direction.z * direction.z);
		float pitch = std::atan2(direction.y, horizontalDistance);

		// Z軸回転（ロール）は不要なので0
		return Vector3(-pitch, yaw, 0.0f);
	}

	Matrix4x4 Transpose(const Matrix4x4& m)
	{
		Matrix4x4 result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = m.m[j][i];
			}
		}
		return result;
	}



}
