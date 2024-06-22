#pragma once
#include <stdint.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl.h>
#include <chrono>

class DirectXCommon
{
public:// メンバ関数

	/// 
	/// Default Method
	/// 

	~DirectXCommon() = default;	

	static DirectXCommon* GetInstance();

	// メインループ前の初期化
	void Initialize(HWND hwnd);

	// 描画前の処理
	void PreDraw();

	// 描画後の処理
	void PostDraw();

	// 解放処理とリソースチェック
	void Finalize();

	///
	/// User Method
	/// 

	// Getter
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return device_.Get(); }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return commandList_.Get(); }
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvDescriptorHeap() { return srvDescriptorHeap_.Get(); }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDsvDescriptorHeap() { return dsvDescriptorHeap_.Get(); }
	D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc() { return depthStencilDesc_; }
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc_; }
	const D3D12_CPU_DESCRIPTOR_HANDLE* GetRtvHandles() { return rtvHandles_; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRTVDescriptorHeap() { return rtvDescriptorHeap_; }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc_; }

	// BufferResourceの作成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	// DescriptorHeapの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(const Microsoft::WRL::ComPtr<ID3D12Device>& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	// DepthStenciltextureの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);
	// dsvDescriptorHeapの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDsvDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

private:// プライベートなメンバ関数

	// 使用するアダプタを決定する
	void GetAdapter();

	// D3D12Deviceの生成
	void CreateD3D12Device();

	// エラーや警告が起きると止まる
	void StopError();

	// コマンドキューの生成
	void CreateComandQueue();
	// コマンドリストの生成
	void CreateComandList();

	// SwapChainの生成
	void CreateSwapChain(HWND hwnd);
	// 二つのswapChainResoucesを取得
	void GetSwapChainResources();

	// CPUで書き込む用のTextureResourceを作りコマンドを積む
	void CreateDepthStencilView();
	// DepthStencilStateの設定
	void SettingDepthStencilState();

	// RTVを作る
	void CreateRTV();

	// FPS固定初期化
	void InitFixFPS();
	// FPS固定更新
	void UpdateFixFPS();

public:
	// 最大SRV数
	static const uint32_t kMaxSRVCount;

private:// メンバ変数
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;

	// エラー
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue_;

	// command
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	// device
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	// rtv
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	// Depth
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

	// swapChain
	UINT backBufferIndex_;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	// fence
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_;
	HANDLE fenceEvent_;

	// barrier
	D3D12_RESOURCE_BARRIER barrier_;

	// 記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;
};

