#include "LineManager.h"

#include <numbers>

#include "manager/CameraManager.h"

void LineManager::Initialize(DirectXCommon* dxCommon, CameraManager* cameraManager)
{
	dxCommon_ = dxCommon;
	cameraManager_ = cameraManager;
	lineCommon_ = std::make_unique<LineCommon>();
	lineCommon_->Initialize(dxCommon_);
	line_ = std::make_unique<Line>();
	line_->Initialize(lineCommon_.get());
}

void LineManager::Clear() {
    line_->Clear();
}

void LineManager::Draw() {
	//頂点データ、行列データの更新
    line_->Update(cameraManager_->GetActiveCamera());
	//描画
    line_->Draw();
	// 描画後にクリア
	Clear();
}

void LineManager::DrawCube(const Vector3& center, float size, const Vector4& color) {
    float halfSize = size / 2.0f;

    // キューブの8つの頂点を計算
    Vector3 vertices[8] = {
        {center.x - halfSize, center.y - halfSize, center.z - halfSize},
        {center.x + halfSize, center.y - halfSize, center.z - halfSize},
        {center.x + halfSize, center.y + halfSize, center.z - halfSize},
        {center.x - halfSize, center.y + halfSize, center.z - halfSize},
        {center.x - halfSize, center.y - halfSize, center.z + halfSize},
        {center.x + halfSize, center.y - halfSize, center.z + halfSize},
        {center.x + halfSize, center.y + halfSize, center.z + halfSize},
        {center.x - halfSize, center.y + halfSize, center.z + halfSize}
    };

    // キューブの12本の辺を追加
    line_->AddLine(vertices[0], vertices[1], color);
    line_->AddLine(vertices[1], vertices[2], color);
    line_->AddLine(vertices[2], vertices[3], color);
    line_->AddLine(vertices[3], vertices[0], color);

    line_->AddLine(vertices[4], vertices[5], color);
    line_->AddLine(vertices[5], vertices[6], color);
    line_->AddLine(vertices[6], vertices[7], color);
    line_->AddLine(vertices[7], vertices[4], color);

    line_->AddLine(vertices[0], vertices[4], color);
    line_->AddLine(vertices[1], vertices[5], color);
    line_->AddLine(vertices[2], vertices[6], color);
    line_->AddLine(vertices[3], vertices[7], color);
}

void LineManager::DrawSphere(const Vector3& center, float radius, const Vector4& color)
{
    const int segments = 12;
    const int rings = 12;

    for (int i = 0; i <= rings; ++i) {
        float theta1 = i * std::numbers::pi_v<float> / rings;
        float theta2 = (i + 1) * std::numbers::pi_v<float> / rings;

        for (int j = 0; j <= segments; ++j) {
            float phi = j * 2 * std::numbers::pi_v<float> / segments;

            Vector3 p1 = {
                center.x + radius * sinf(theta1) * cosf(phi),
                center.y + radius * cosf(theta1),
                center.z + radius * sinf(theta1) * sinf(phi)
            };

            Vector3 p2 = {
                center.x + radius * sinf(theta2) * cosf(phi),
                center.y + radius * cosf(theta2),
                center.z + radius * sinf(theta2) * sinf(phi)
            };

            line_->AddLine(p1, p2, color);
        }
    }

    for (int i = 0; i <= segments; ++i) {
        float phi1 = i * 2 * std::numbers::pi_v<float> / segments;
        float phi2 = (i + 1) * 2 * std::numbers::pi_v<float> / segments;

        for (int j = 0; j <= rings; ++j) {
            float theta = j * std::numbers::pi_v<float> / rings;

            Vector3 p1 = {
                center.x + radius * sinf(theta) * cosf(phi1),
                center.y + radius * cosf(theta),
                center.z + radius * sinf(theta) * sinf(phi1)
            };

            Vector3 p2 = {
                center.x + radius * sinf(theta) * cosf(phi2),
                center.y + radius * cosf(theta),
                center.z + radius * sinf(theta) * sinf(phi2)
            };

            line_->AddLine(p1, p2, color);
        }
    }
}

void LineManager::Drawline(const Vector3& start, const Vector3& end, const Vector4& color)
{
	line_->AddLine(start, end, color);
}

void LineManager::DrawGrid(float gridSize, float gridSpacing, const Vector4& color)
{
    // グリッドの範囲を計算
    float halfSize = gridSize / 2.0f;

    // X軸方向の線を描画
    for (float z = -halfSize; z <= halfSize; z += gridSpacing) {
        Vector3 start = { -halfSize, 0.0f, z };
        Vector3 end = { halfSize, 0.0f, z };
        Drawline(start, end, color);
    }

    // Z軸方向の線を描画
    for (float x = -halfSize; x <= halfSize; x += gridSpacing) {
        Vector3 start = { x, 0.0f, -halfSize };
        Vector3 end = { x, 0.0f, halfSize };
        Drawline(start, end, color);
    }
}

