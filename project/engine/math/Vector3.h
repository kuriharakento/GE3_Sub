#pragma once
#include <cmath>

struct Vector3 {
    float x, y, z;

    Vector3 Normalize() const {
        float len = Length();
        if (len == 0) return Vector3{ 0.0f, 0.0f, 0.0f };
        return *this / len;
    }

    float Length() const {
        return sqrtf(x * x + y * y + z * z);
    }

    float Dot(const Vector3& other) const {
        return (x * other.x) + (y * other.y) + (z * other.z);
    }

    Vector3 Cross(const Vector3& other) const {
        return Vector3{
            (y * other.z) - (z * other.y),
            (z * other.x) - (x * other.z),
            (x * other.y) - (y * other.x)
        };
    }

    static Vector3 Normalize(const Vector3& vec) {
        float len = vec.Length();
        if (len == 0) return Vector3{ 0.0f, 0.0f, 0.0f };
        return vec / len;
    }

    static float Length(const Vector3& vec) {
        return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
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
        return Vector3{ -this->x, -this->y, -this->z };
    }
};
