#pragma once
#include <cmath> // 標準ライブラリのcmathをインクルード

class Vector2; // 前方宣言
class Vector3; // 前方宣言
class Vector4; // 前方宣言

/**
 * @class Vector2
 * @brief 2Dベクトルを表すクラス
 *
 * このクラスは、2D空間でのベクトル演算を行うための基本的な機能を提供します。
 */
class Vector2 {
public:
    float x; ///< X成分
    float y; ///< Y成分

    /// \brief デフォルトコンストラクタ
    /// 初期値は(0, 0)
    Vector2() : x(0), y(0) {}

    /// \brief 指定したXおよびY成分で初期化するコンストラクタ
    /// \param x 初期X成分
    /// \param y 初期Y成分
    Vector2(float x, float y) : x(x), y(y) {}

    /// \brief ベクトル同士の加算演算子
    /// \param other 加算対象のベクトル
    /// \return 加算結果の新しいベクトル
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    /// \brief ベクトル同士の減算演算子
    /// \param other 減算対象のベクトル
    /// \return 減算結果の新しいベクトル
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    /// \brief スカラー値との乗算
    /// \param scalar 乗算するスカラー値
    /// \return 乗算結果の新しいベクトル
    Vector2 Multiply(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    /// \brief スカラー値による除算
    /// \param scalar 除算するスカラー値
    /// \return 除算結果の新しいベクトル
    Vector2 Divide(float scalar) const {
        return Vector2(x / scalar, y / scalar);
    }

    /// \brief ドット積の計算
    /// \param other ドット積を計算する対象のベクトル
    /// \return ドット積の結果
    float Dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    /// \brief ベクトルの長さを計算
    /// \return ベクトルの長さ
    float Length() const {
        return std::sqrt(x * x + y * y);
    }

    /// \brief ベクトルを正規化
    /// \return 正規化された新しいベクトル
    Vector2 Normalize() const {
        float len = Length();
        return len != 0 ? Divide(len) : Vector2(0, 0);
    }
};

/**
 * @class Vector3
 * @brief 3Dベクトルを表すクラス
 *
 * このクラスは、3D空間でのベクトル演算を行うための基本的な機能を提供します。
 */
class Vector3 {
public:
    float x; ///< X成分
    float y; ///< Y成分
    float z; ///< Z成分

    /// \brief デフォルトコンストラクタ
    /// 初期値は(0, 0, 0)
    Vector3() : x(0), y(0), z(0) {}

    /// \brief 指定したX、Y、Z成分で初期化するコンストラクタ
    /// \param x 初期X成分
    /// \param y 初期Y成分
    /// \param z 初期Z成分
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    /// \brief ベクトル同士の加算演算子
    /// \param other 加算対象のベクトル
    /// \return 加算結果の新しいベクトル
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    /// \brief ベクトル同士の減算演算子
    /// \param other 減算対象のベクトル
    /// \return 減算結果の新しいベクトル
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    /// \brief スカラー値との乗算
    /// \param scalar 乗算するスカラー値
    /// \return 乗算結果の新しいベクトル
    Vector3 Multiply(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    /// \brief スカラー値による除算
    /// \param scalar 除算するスカラー値
    /// \return 除算結果の新しいベクトル
    Vector3 Divide(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    /// \brief ドット積の計算
    /// \param other ドット積を計算する対象のベクトル
    /// \return ドット積の結果
    float Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    /// \brief クロス積の計算
    /// \param other クロス積を計算する対象のベクトル
    /// \return クロス積の結果
    Vector3 Cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    /// \brief ベクトルの長さを計算
    /// \return ベクトルの長さ
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    /// \brief ベクトルを正規化
    /// \return 正規化された新しいベクトル
    Vector3 Normalize() const {
        float len = Length();
        return len != 0 ? Divide(len) : Vector3(0, 0, 0);
    }
};

/**
 * @class Vector4
 * @brief 4Dベクトルを表すクラス
 *
 * このクラスは、4D空間でのベクトル演算を行うための基本的な機能を提供します。
 */
class Vector4 {
public:
    float x; ///< X成分
    float y; ///< Y成分
    float z; ///< Z成分
    float w; ///< W成分

    /// \brief デフォルトコンストラクタ
    /// 初期値は(0, 0, 0, 0)
    Vector4() : x(0), y(0), z(0), w(0) {}

    /// \brief 指定したX、Y、Z、W成分で初期化するコンストラクタ
    /// \param x 初期X成分
    /// \param y 初期Y成分
    /// \param z 初期Z成分
    /// \param w 初期W成分
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    /// \brief ベクトル同士の加算演算子
    /// \param other 加算対象のベクトル
    /// \return 加算結果の新しいベクトル
    Vector4 operator+(const Vector4& other) const {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    /// \brief ベクトル同士の減算演算子
    /// \param other 減算対象のベクトル
    /// \return 減算結果の新しいベクトル
    Vector4 operator-(const Vector4& other) const {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    /// \brief スカラー値との乗算
    /// \param scalar 乗算するスカラー値
    /// \return 乗算結果の新しいベクトル
    Vector4 Multiply(float scalar) const {
        return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    /// \brief スカラー値による除算
    /// \param scalar 除算するスカラー値
    /// \return 除算結果の新しいベクトル
    Vector4 Divide(float scalar) const {
        return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
    }

    /// \brief ドット積の計算
    /// \param other ドット積を計算する対象のベクトル
    /// \return ドット積の結果
    float Dot(const Vector4& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    /// \brief ベクトルの長さを計算
    /// \return ベクトルの長さ
    float Length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    /// \brief ベクトルを正規化
    /// \return 正規化された新しいベクトル
    Vector4 Normalize() const {
        float len = Length();
        return len != 0 ? Divide(len) : Vector4(0, 0, 0, 0);
    }
};
