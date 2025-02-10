#pragma once
#include "LineCommon.h"
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include "base/GraphicsTypes.h"

class Camera;

class Line {
public:
    Line() = default;
    void Initialize(LineCommon* lineCommon);
    void AddLine(const Vector3& start, const Vector3& end, const Vector4& color);
	void Update(Camera* camera);
	void Draw();
    void Clear();

private:
    void CreateVertexData();
	void CreateWVPResource();

    LineCommon* lineCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    std::vector<LineVertex> vertices_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    LineVertex* vertexData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;

	Matrix4x4 worldMatrix_ = MakeIdentity4x4();

};
