#pragma once
#include <cassert>
#include <cmath>

// Vector2
struct Vector2 {
    float x, y;

    // インスタンスメソッド
    Vector2 Normalize() const {
        float len = Length();
        if (len == 0) return Vector2{ 0.0f, 0.0f };
        return *this / len;
    }

    float Length() const {
        return sqrtf(x * x + y * y);
    }

    float Dot(const Vector2& other) const {
        return (x * other.x) + (y * other.y);
    }

    // スタティックメソッド
    static Vector2 Normalize(const Vector2& vec) {
        float len = vec.Length();
        if (len == 0) return Vector2{ 0.0f, 0.0f };
        return vec / len;
    }

    static float Length(const Vector2& vec) {
        return sqrtf(vec.x * vec.x + vec.y * vec.y);
    }

    static float Dot(const Vector2& a, const Vector2& b) {
        return (a.x * b.x) + (a.y * b.y);
    }

    // オペレータ
    Vector2 operator+(const Vector2& other) const {
        return Vector2{ x + other.x, y + other.y };
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2{ x - other.x, y - other.y };
    }

    Vector2 operator*(float scalar) const {
        return Vector2{ x * scalar, y * scalar };
    }

    Vector2 operator/(float scalar) const {
        return Vector2{ x / scalar, y / scalar };
    }

    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
};

// Vector3
struct Vector3 {
    float x, y, z;

    // インスタンスメソッド
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

    // スタティックメソッド
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

    // オペレータ
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

// Vector4
struct Vector4 {
    float x, y, z, w;

    // インスタンスメソッド
    Vector4 Normalize() const {
        float len = Length();
        if (len == 0) return Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };
        return *this / len;
    }

    float Length() const {
        return sqrtf(x * x + y * y + z * z + w * w);
    }

    float Dot(const Vector4& other) const {
        return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
    }

    // スタティックメソッド
    static Vector4 Normalize(const Vector4& vec) {
        float len = vec.Length();
        if (len == 0) return Vector4{ 0.0f, 0.0f, 0.0f, 0.0f };
        return vec / len;
    }

    static float Length(const Vector4& vec) {
        return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
    }

    static float Dot(const Vector4& a, const Vector4& b) {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    }

    // オペレータ
    Vector4 operator+(const Vector4& other) const {
        return Vector4{ x + other.x, y + other.y, z + other.z, w + other.w };
    }

    Vector4 operator-(const Vector4& other) const {
        return Vector4{ x - other.x, y - other.y, z - other.z, w - other.w };
    }

    Vector4 operator*(float scalar) const {
        return Vector4{ x * scalar, y * scalar, z * scalar, w * scalar };
    }

    Vector4 operator/(float scalar) const {
        return Vector4{ x / scalar, y / scalar, z / scalar, w / scalar };
    }

    Vector4& operator+=(const Vector4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    Vector4& operator-=(const Vector4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
};
