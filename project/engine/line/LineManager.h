#pragma once
#include "Line.h"
#include "LineCommon.h"
#include <memory>
#include <vector>

class CameraManager;

struct Cube {
    Vector3 center;
    float size;
    Vector4 color;
    std::vector<LineVertex> vertices; // キューブの頂点データ
};

class LineManager {
public:
    void Initialize(DirectXCommon* dxCommon,CameraManager* cameraManager);
    void Update();
    void Draw();

    /**
     * @brief 線を追加
     * @param start 線の始点
     * @param end 線の終点
     * @param color 線の色
     */
    void AddLine(const Vector3& start, const Vector3& end, const Vector4& color);
    /**
     * @brief 線をクリア
     */
    void Clear();
    /**
     * @brief キューブを線で作成
     * @param center キューブの中心座標
     * @param size キューブのサイズ
     * @param color 線の色
     */
    void AddCube(const Vector3& center, float size, const Vector4& color);

    /**
     * @brief キューブを描画
     * @param center キューブの中心座標
     * @param size キューブのサイズ
     * @param color 線の色
     */
    void DrawCube(const Vector3& center, float size, const Vector4& color);

private:
    std::unique_ptr<LineCommon> lineCommon_; ///< LineCommon クラスのインスタンス
    std::unique_ptr<Line> line_;             ///< Line クラスのインスタンス
    DirectXCommon* dxCommon_ = nullptr;      ///< DirectXCommon クラスのインスタンス
	CameraManager* cameraManager_ = nullptr; ///< CameraManager クラスのインスタンス

    std::vector<Cube> cubes_;                ///< キューブのリスト

    void CreateCubeVertices(Cube& cube);     ///< キューブの頂点データを作成
};
