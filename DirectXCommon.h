#pragma once
#include <array>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "WinApp.h"

/*--------------[  ]-----------------*/


class DirectXCommon
{
public: //メンバ関数
	//初期化
	void Initialize(WinApp *winApp);

	/// \brief SRVのCPUディスクリプタハンドルを取得する
	/// \param index 
	/// \return 
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	/// \brief SRVのGPUディスクリプタハンドルを取得する
	/// \param index 
	/// \return 
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

private: //メンバ関数
	/// \brief デバイスの初期化
	void InitializeDevice();
	/// \brief コマンド関連の初期化
	void InitializeCommand();
	/// \brief スワップチェインの生成
	void CreateSwapChain();
	//// \brief 深度バッファの生成
	void CreateDepthBuffer();
	// 各種ディスクリプタの生成
	void CreateDescriptorHeap();
	/// \brief レンダーターゲットビューの生成
	void CreateRenderTargetView();
	//深度ステンシルビューの初期化
	void CreateDepthStencilView();
	/// \brief フェンスの生成
	void CreateFence();
	/// \brief ビューポート矩形の初期化
	void InitializeViewPort();
	/// \brief シザリング矩形の初期化
	void InitializeScissorRect();
	// DXCコンパイラの初期化
	void InitializeDXCCompiler();
	//ImGuiの初期化
	void InitializeImGui();


	//ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptor, bool shaderVisible);

	/// \brief CPUのDescriptorHandleを取得
	/// \param descriptorHeap 
	/// \param descriptorSize 
	/// \param index 
	/// \return 
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/// \brief GPUのDescriptorHandleを取得
	/// \param descriptorHeap 
	/// \param descriptorSize 
	/// \param index 
	/// \return 
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);


private: //メンバ変数
	//WindowsAPIの
	WinApp* winApp_ = nullptr;

	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	//DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;

	//コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	//スワップチェイン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	//スワップチェインリソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources_;

	//深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_ = nullptr;

	//ディスクリプタヒープ
	//RTV
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	//SRV
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	//DSV
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;

	//ディスクリプタサイズ
	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;

	//ビューポート
	D3D12_VIEWPORT viewport_{};

	//シザー矩形
	D3D12_RECT scissorRect_{};

	
	//DXCユーティリティ
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	//DXCコンパイラ
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	//インクルードハンドラ
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
};

