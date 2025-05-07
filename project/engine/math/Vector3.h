#pragma once
#include <cmath>

struct Vector3 {
    float x, y, z;

    // --- 基本処理 ---
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float LengthSquared() const {
        return x * x + y * y + z * z;
    }

    Vector3 Normalize() const {
        float len = Length();
        if (len == 0.0f) return Vector3{ 0.0f, 0.0f, 0.0f };
        return *this / len;
    }

    bool IsZero(float epsilon = 1e-6f) const {
        return LengthSquared() < epsilon * epsilon;
    }

    // --- 静的関数 ---
    static Vector3 Normalize(const Vector3& vec) {
        return vec.Normalize();
    }

    static float Length(const Vector3& vec) {
        return vec.Length();
    }

    static float LengthSquared(const Vector3& vec) {
        return vec.LengthSquared();
    }

    static float Dot(const Vector3& a, const Vector3& b) {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    }

    static Vector3 Cross(const Vector3& a, const Vector3& b) {
        return Vector3{
            (a.y * b.z) - (a.z * b.y),
            (a.z * b.x) - (a.x * b.z),
            (a.x * b.y) - (a.y * b.x)
        };
    }

    static float Distance(const Vector3& a, const Vector3& b) {
        return (a - b).Length();
    }

    static float DistanceSquared(const Vector3& a, const Vector3& b) {
        return (a - b).LengthSquared();
    }

    // --- 演算子 ---
    Vector3 operator+(const Vector3& other) const {
        return Vector3{ x + other.x, y + other.y, z + other.z };
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3{ x - other.x, y - other.y, z - other.z };
    }

    Vector3 operator*(const Vector3& other) const {
        return Vector3{ x * other.x, y * other.y, z * other.z };
    }

    Vector3 operator*(float scalar) const {
        return Vector3{ x * scalar, y * scalar, z * scalar };
    }

    Vector3 operator/(float scalar) const {
        return Vector3{ x / scalar, y / scalar, z / scalar };
    }

    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vector3 operator-() const {
        return Vector3{ -x, -y, -z };
    }
};
