#include "LineManager.h"

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

void LineManager::AddLine(const Vector3& start, const Vector3& end, const Vector4& color) {
    line_->AddLine(start, end, color);
}

void LineManager::Clear() {
    line_->Clear();
}

void LineManager::Update() {
	line_->Update(cameraManager_->GetActiveCamera());
}

void LineManager::Draw() {
    line_->Draw();
}

void LineManager::AddCube(const Vector3& center, float size, const Vector4& color) {
    Cube cube = { center, size, color };
    CreateCubeVertices(cube);
    cubes_.push_back(cube);
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
    AddLine(vertices[0], vertices[1], color);
    AddLine(vertices[1], vertices[2], color);
    AddLine(vertices[2], vertices[3], color);
    AddLine(vertices[3], vertices[0], color);

    AddLine(vertices[4], vertices[5], color);
    AddLine(vertices[5], vertices[6], color);
    AddLine(vertices[6], vertices[7], color);
    AddLine(vertices[7], vertices[4], color);

    AddLine(vertices[0], vertices[4], color);
    AddLine(vertices[1], vertices[5], color);
    AddLine(vertices[2], vertices[6], color);
    AddLine(vertices[3], vertices[7], color);
}

void LineManager::CreateCubeVertices(Cube& cube) {
    float halfSize = cube.size / 2.0f;

    // キューブの8つの頂点を計算
    Vector3 vertices[8] = {
        {cube.center.x - halfSize, cube.center.y - halfSize, cube.center.z - halfSize},
        {cube.center.x + halfSize, cube.center.y - halfSize, cube.center.z - halfSize},
        {cube.center.x + halfSize, cube.center.y + halfSize, cube.center.z - halfSize},
        {cube.center.x - halfSize, cube.center.y + halfSize, cube.center.z - halfSize},
        {cube.center.x - halfSize, cube.center.y - halfSize, cube.center.z + halfSize},
        {cube.center.x + halfSize, cube.center.y - halfSize, cube.center.z + halfSize},
        {cube.center.x + halfSize, cube.center.y + halfSize, cube.center.z + halfSize},
        {cube.center.x - halfSize, cube.center.y + halfSize, cube.center.z + halfSize}
    };

    // キューブの12本の辺を追加
    cube.vertices = {
        {vertices[0], cube.color}, {vertices[1], cube.color},
        {vertices[1], cube.color}, {vertices[2], cube.color},
        {vertices[2], cube.color}, {vertices[3], cube.color},
        {vertices[3], cube.color}, {vertices[0], cube.color},
        {vertices[4], cube.color}, {vertices[5], cube.color},
        {vertices[5], cube.color}, {vertices[6], cube.color},
        {vertices[6], cube.color}, {vertices[7], cube.color},
        {vertices[7], cube.color}, {vertices[4], cube.color},
        {vertices[0], cube.color}, {vertices[4], cube.color},
        {vertices[1], cube.color}, {vertices[5], cube.color},
        {vertices[2], cube.color}, {vertices[6], cube.color},
        {vertices[3], cube.color}, {vertices[7], cube.color}
    };
}
