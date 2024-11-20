#pragma once
#include <d3d12.h>
#include <string>
#include <wrl.h>

#include "base/DirectXCommon.h"

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
	void Initialize(DirectXCommon* dxCommon);
	/// \brief テクスチャの読み込み
	void LoadTexture(const std::string& filePath);
	/// \brief SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);
	/// \brief テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);
	/// \brief メタデータ取得
	const DirectX::TexMetadata& GetMetadata(uint32_t textureIndex);
private: //構造体
	/// \brief テクスチャデータ
	struct TextureData
	{
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediate;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	std::vector<TextureData> textureDatas_;

private: //メンバ関数
	
	/**
	 * \brief 
	 * \param texture テクスチャリソースの転送
	 * \param mipImages 
	 * \return 
	 */
	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);


private: //メンバ変数
	//DirectXコマンド
	DirectXCommon* dxCommon_ = nullptr;

	//SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

private: //シングルトンインスタンス
	static TextureManager* instance_;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;



};

