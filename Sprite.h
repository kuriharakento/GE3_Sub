#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>


#include "MatrixFunc.h"
#include "VectorFunc.h"

//ポインタが必要なので前方宣言
class SpriteCommon;

//スプライト
class Sprite
{
public: //メンバ関数
	/// \brief 初期化
	void Initialize(SpriteCommon* spriteCommon);

	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct Material
	{
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

private: //メンバ関数
	/// \brief 頂点データ作成
	void CreateVertexData();

private: //メンバ変数
	
	SpriteCommon* spriteCommon_ = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

};

