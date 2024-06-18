#pragma once
#include "DirectXCommon.h"
#include "DirectXTex.h"

class SrvManager
{
public:
	/// 
	/// Default Method
	/// 

	static SrvManager* GetInstance();
	void Initialize();
	//void Update();
	//void Finalize();
	void PreDraw();

	uint32_t Allocate();

	/// 
	/// User Method
	/// 

	/// SRV作成関数

	// テクスチャ用
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT mipLevels, DirectX::TexMetadata metdata);
	// Structured Buffer用
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
	// ポストエフェクト用
	void CreateSRVforPostEffect(uint32_t srvIndex, ID3D12Resource* pResource);
	// Depth用
	void CreateSRVforDepth(uint32_t srvIndex, ID3D12Resource* pResource);

	//*** Getter ***//

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index) {
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += (descriptorSize_ * index);
		return handleCPU;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index) {
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += (descriptorSize_ * index);
		return handleGPU;
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return descriptorHeap_.Get(); }

	// 読み込み可能数チェック
	bool GetIsLimit() {
		if (useIndex_ > kMaxSRVCount) {
			return false;
		}
		return true;
	}

	//*** Setter ***//

	void SetGraphicsRootDesctiptorTable(UINT rootParameterIndex, uint32_t srvIndex);

private:// 定数
	// 最大SRV数
	static const uint32_t kMaxSRVCount;

private:
	DirectXCommon* directXCommon_;

	// SRV用のデスクリプタサイズ
	uint32_t descriptorSize_;
	// SRV用のデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	// 次に使用するSRVインデックス
	uint32_t useIndex_ = 0;
};