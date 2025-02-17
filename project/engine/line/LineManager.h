#pragma once
#include "Line.h"
#include "LineCommon.h"
#include <memory>
#include <vector>

class CameraManager;

class LineManager {
public:
    void Initialize(DirectXCommon* dxCommon,CameraManager* cameraManager);
    void Draw();
    void Clear();
	//ラインの描画
	void Drawline(const Vector3& start, const Vector3& end, const Vector4& color);
	//キューブの描画
    void DrawCube(const Vector3& center, float size, const Vector4& color);
	//球の描画
	void DrawSphere(const Vector3& center, float radius, const Vector4& color);
private:
    std::unique_ptr<LineCommon> lineCommon_; ///< LineCommon クラスのインスタンス
    std::unique_ptr<Line> line_;             ///< Line クラスのインスタンス
    DirectXCommon* dxCommon_ = nullptr;      ///< DirectXCommon クラスのインスタンス
	CameraManager* cameraManager_ = nullptr; ///< CameraManager クラスのインスタンス
};
