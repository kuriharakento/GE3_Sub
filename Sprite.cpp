#include "Sprite.h"

#include "SpriteCommon.h"
##include "SpriteCommon.h"
void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	//引数で受け取ってメンバ変数に記録する
	spriteCommon_ = spriteCommon;

	//頂点データを作成する
	CreateVertexData();

}

void Sprite::CreateVertexData()
{
	/*--------------[ VertexResourceを作る ]-----------------*/

	
	vertexResource_ = spriteCommon_->GetDXCommon()->CreateBufferResource(sizeof(VertexData) * 1536);


	/*--------------[ IndexResourceを作る ]-----------------*/

	indexResource_ = spriteCommon_->GetDXCommon()->CreateBufferResource(sizeof(uint32_t) * 6);

	/*--------------[ VertexBufferViewを作成する（値を設定するだけ） ]-----------------*/

	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点３つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 1536;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	/*--------------[ IndexBufferViewを作成する（値を設定するだけ） ]-----------------*/

	///===================================================================
	///IndexBufferViewを生成する
	///===================================================================

	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズをインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	/*--------------[ VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる ]-----------------*/

	vertexResource_->Map(
		0,
		nullptr,
		reinterpret_cast<void**>(&vertexData_)
	);

	/*--------------[ IndexResourceにデータを書き込むためのアドレスを取得してindexDataに割り当てる ]-----------------*/

	indexResource_->Map(
		0, 
		nullptr, 
		reinterpret_cast<void**>(&indexData_)
	);

}
