#pragma once
#include "Sprite.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include <Windows.h>

class IPostEffect {
protected:// 構造体
	struct RenderingTextureData {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IPostEffect() = default;
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IPostEffect() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	//void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr);
	/// <summary>
	/// 描画前処理
	/// </summary>
	virtual void PreDrawScene();
	/// <summary>
	/// 描画後の処理
	/// </summary>
	virtual void PostDrawScene();

#pragma region Getter
	// レンダーテクスチャの取得
	uint32_t GetRenderTexture() { return texBuff_.srvIndex; }
	// ポストエフェクトを使用するかを取得
	bool GetIsActive() { return isActive_; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
#pragma endregion

#pragma region Setter

#pragma endregion

protected:
	/// <summary>
	/// スプライトの初期化(現状のSpriteクラスだとテクスチャの読み込みをしなくてはいけないので新たに作る)
	/// </summary>
	void SpriteInitialize(RenderingTextureData texData);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	void CreateVertexResource();

	void CreateVertexBufferView();

	void CreateIndexResource();

	void CreateIndexBufferView();

	void CreateMaterialResource();

	// テクスチャを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureBufferResource();

	// SRVの作成
	RenderingTextureData CreateSRV(RenderingTextureData texData);

	// RTVの作成
	void CreateRTV(RenderingTextureData texData, uint32_t index);

	// バリアを張る
	void SetBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> texBuff, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState, UINT numBarrier);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateRTVDescriptorHeap(const Microsoft::WRL::ComPtr<ID3D12Device>& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

public:// 定数
	// 画面クリアカラー
	static const float clearColor_[4];

public:// パブリックな変数

protected:// プライベートな変数
	// 基本機能
	DirectXCommon* directXCommon_;

	// テクスチャバッファ
	RenderingTextureData texBuff_;
	// depth用のテクスチャ
	RenderingTextureData depthTexBuff_;

	// 深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuff_;
	// RTV用のデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapRTV_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	// DSV用のデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapDSV_;

#pragma region スプライト
	WorldTransform worldTransform_;
	// カメラ
	std::unique_ptr<Camera> camera_;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraPosResource_;
	Vector3 cameraPosData_;

	// Material
	Material* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Transform uvTransform_;
	Matrix4x4 uvTransformMatrix_;
	// Vertex
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_;
	// index
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	uint32_t* indexData_ = nullptr;
	// Sprite

	ViewProjection viewProjection_;

	// 画像の中心点
	Vector2 anchorPoint_ = { 0.5f,0.5f };

	// スプライトの縦幅、横幅
	Vector2 size_;

	// 画像の左上の座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャの切り出しサイズ
	Vector2 textureSize_;

	uint32_t textureIndex_;
#pragma endregion

	// ポストエフェクトを使用するか
	bool isActive_;
};