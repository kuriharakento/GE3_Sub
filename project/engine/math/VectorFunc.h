#pragma once
#include <cmath>

struct Vector2 {
    float x; ///< X成分
    float y; ///< Y成分

    // 加算オペレータ
    Vector2 operator+(const Vector2& other) const {
        return Vector2{ x + other.x, y + other.y };
    }

    // 減算オペレータ
    Vector2 operator-(const Vector2& other) const {
        return Vector2{ x - other.x, y - other.y };
    }

    // スカラー乗算
    Vector2 operator*(float scalar) const {
        return Vector2{ x * scalar, y * scalar };
    }

    // スカラー除算
    Vector2 operator/(float scalar) const {
        return Vector2{ x / scalar, y / scalar };
    }

    // 加算代入
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // 減算代入
    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
};

struct Vector3 {
    float x; ///< X成分
    float y; ///< Y成分
    float z; ///< Z成分

    // 加算オペレータ
    Vector3 operator+(const Vector3& other) const {
        return Vector3{ x + other.x, y + other.y, z + other.z };
    }

    // 減算オペレータ
    Vector3 operator-(const Vector3& other) const {
        return Vector3{ x - other.x, y - other.y, z - other.z };
    }

    // スカラー乗算
    Vector3 operator*(float scalar) const {
        return Vector3{ x * scalar, y * scalar, z * scalar };
    }

    // スカラー除算
    Vector3 operator/(float scalar) const {
        return Vector3{ x / scalar, y / scalar, z / scalar };
    }

    // 加算代入
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // 減算代入
    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
};

// Vector3の長さを計算
inline float Length(const Vector3& v) {
    return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// Vector3を正規化
inline Vector3 Normalize(const Vector3& v) {
    float length = Length(v);
    return Vector3{ v.x / length, v.y / length, v.z / length };
}

struct Vector4 {
    float x; ///< X成分
    float y; ///< Y成分
    float z; ///< Z成分
    float w; ///< W成分

    // 加算オペレータ
    Vector4 operator+(const Vector4& other) const {
        return Vector4{ x + other.x, y + other.y, z + other.z, w + other.w };
    }

    // 減算オペレータ
    Vector4 operator-(const Vector4& other) const {
        return Vector4{ x - other.x, y - other.y, z - other.z, w - other.w };
    }

    // スカラー乗算
    Vector4 operator*(float scalar) const {
        return Vector4{ x * scalar, y * scalar, z * scalar, w * scalar };
    }

    // スカラー除算
    Vector4 operator/(float scalar) const {
        return Vector4{ x / scalar, y / scalar, z / scalar, w / scalar };
    }

    // 加算代入
    Vector4& operator+=(const Vector4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    // 減算代入
    Vector4& operator-=(const Vector4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
};
