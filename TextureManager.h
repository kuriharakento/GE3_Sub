#pragma once
#include <d3d12.h>
#include <string>
#include <wrl/client.h>

#include "externals/DirectXTex/DirectXTex.h"

/**
 * \brief テクスチャマネージャー
 */
class TextureManager
{
public:
	/// \brief インスタンス取得
	static TextureManager* GetInstance();
	/// \brief インスタンス解放
	void Finalize();
	/// \brief 初期化
	void Initialize();
	/// \brief テクスチャの読み込み
	void LoadTexture(const std::string& filePath);
private: //構造体
	/// \brief テクスチャデータ
	struct TextureData
	{
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHadleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	std::vector<TextureData> textureDatas_;

private: //メンバ関数
	static TextureManager* instance_;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;



};

