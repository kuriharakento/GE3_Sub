#include "Line.h"
#include "base/DirectXCommon.h"
#include "base/Camera.h"

void Line::Initialize(LineCommon* lineCommon) {
    lineCommon_ = lineCommon;
	CreateVertexData();
    CreateWVPResource();
}

void Line::AddLine(const Vector3& start, const Vector3& end, const Vector4& color) {
    vertices_.push_back({ start, color });
    vertices_.push_back({ end, color });
}

void Line::Update(Camera* camera)
{
    UpdateVertexData();
	UpdateMatrix(camera);
}

void Line::CreateVertexData() {
    if (vertices_.empty()) return;

    // バッファリソースの作成
    vertexResource_ = lineCommon_->GetDirectXCommon()->CreateBufferResource(
        sizeof(LineVertex) * kMaxVertexCount
    );

    // バッファにデータを書き込む
    void* mappedData = nullptr;
    vertexResource_->Map(0, nullptr, &mappedData);
    std::memcpy(mappedData, vertices_.data(), sizeof(LineVertex) * vertices_.size());
    vertexResource_->Unmap(0, nullptr);  // ← Unmap を追加！

    // バッファビューの設定
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(LineVertex) * static_cast<UINT>(vertices_.size());
    vertexBufferView_.StrideInBytes = sizeof(LineVertex);
}

void Line::CreateWVPResource()
{
    // 定数バッファの作成
    wvpResource_ = lineCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(Matrix4x4));

    // 初期データ（単位行列）を設定
    Matrix4x4 identityMatrix = MakeIdentity4x4();
    void* mappedData = nullptr;
    wvpResource_->Map(0, nullptr, &mappedData);
    std::memcpy(mappedData, &identityMatrix, sizeof(Matrix4x4));
    wvpResource_->Unmap(0, nullptr);
}

void Line::UpdateVertexData()
{
	// バッファにデータを書き込む
	void* mappedData = nullptr;
	vertexResource_->Map(0, nullptr, &mappedData);
	std::memcpy(mappedData, vertices_.data(), sizeof(LineVertex) * vertices_.size());
	vertexResource_->Unmap(0, nullptr);
}

void Line::UpdateMatrix(Camera* camera)
{
    if (!camera) { return; }

    // カメラのビュー・プロジェクション行列を取得
    Matrix4x4 viewProjectionMatrix = camera->GetViewProjectionMatrix();

    // ワールド行列とビュー・プロジェクション行列を掛け合わせる
    Matrix4x4 wvpMatrix = worldMatrix_ * viewProjectionMatrix;

    // 定数バッファに書き込む
    void* mappedData = nullptr;
    HRESULT hr = wvpResource_->Map(0, nullptr, &mappedData);
    if (FAILED(hr) || mappedData == nullptr) {
        return; // メモリ確保失敗時はスキップ
    }
    std::memcpy(mappedData, &wvpMatrix, sizeof(Matrix4x4));
    wvpResource_->Unmap(0, nullptr);
}

void Line::Draw() {
    if (vertices_.empty() || !vertexResource_ || !wvpResource_) return;

    auto commandList = lineCommon_->GetDirectXCommon()->GetCommandList();
    commandList->SetPipelineState(lineCommon_->GetPipelineState().Get());
    commandList->SetGraphicsRootSignature(lineCommon_->GetRootSignature().Get());

    // バッファが有効かチェック
    if (vertexBufferView_.BufferLocation == 0) return;

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

    // WVP行列をGPUに送る
    commandList->SetGraphicsRootConstantBufferView(0, wvpResource_->GetGPUVirtualAddress());

    // 描画
    commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);
}

void Line::Clear() {
    vertices_.clear();
	CreateVertexData();
}
