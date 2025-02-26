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
	size_t GetLineCount() const { return line_->GetLineCount(); }
	//ラインの描画
	void DrawLine(const Vector3& start, const Vector3& end, const Vector4& color);
	//キューブの描画
    void DrawCube(const Vector3& center, float size, const Vector4& color);
	//球の描画
	void DrawSphere(const Vector3& center, float radius, const Vector4& color);
	//グリッドの描画
    void DrawGrid(float size, float step, const Vector4& color);
	//モデルの頂点からラインを描画
	void DrawModel(const std::vector<VertexData>& vertices, const Vector3& center, const Vector4& color);
private:
    std::unique_ptr<LineCommon> lineCommon_; ///< LineCommon クラスのインスタンス
    std::unique_ptr<Line> line_;             ///< Line クラスのインスタンス
    DirectXCommon* dxCommon_ = nullptr;      ///< DirectXCommon クラスのインスタンス
	CameraManager* cameraManager_ = nullptr; ///< CameraManager クラスのインスタンス
};
