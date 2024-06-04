#pragma once
#include "d3dx12.h"
#include "DirectXTex.h"
#include "DirectXCommon.h"
#include "MathStructs.h"
#include "ModelStructs.h"
#include "SrvManager.h"
#include <string>
#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>

enum TextureName {
	UVCHEKER,
	MONSTERBALL,
	TESTBLOCK,
	PARTICLE,
	COUNTTEXTURE	// テクスチャ数を数える用
};

class TextureManager
{
private:
	// テクスチャ1枚分のデータ
	struct TextureData {
		//std::string filePath;
		DirectX::TexMetadata metdata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

public:
	// シングルトン(ユーザー使用禁止)
	static TextureManager* GetInstance();

	// デストラクタ
	~TextureManager() = default;

	// 初期化(ユーザー使用禁止)
	void Initialize(SrvManager* srvManager);

	// 解放処理(ユーザー使用禁止)
	void Finalize();

	// COMの終了処理(ユーザー使用禁止)
	void ComUninit();
	// COMの初期化(ユーザー使用禁止)
	void ComInit();

	// Textureを読む
	void LoadTexture(const std::string& filePath);
	void LoadTexture(const std::string& directoryPath, const std::string& fileName);

	/// Getter
	//D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);
	//D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& directoryPath, const std::string& fileName);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);




	/// <summary>
	/// SRVインデックスの取得
	/// </summary>
	/// <param name="filePath">Engine/resource~~テクスチャの名前までの全てのパス</param>
	/// <returns></returns>
	uint32_t GetSrvIndex(const std::string& filePath);
	/// <summary>
	/// SRVインデックスの取得
	/// </summary>
	/// <param name="directoryPath">テクスチャの入っているフォルダの名前(Engine/resources/は書く必要なし)</param>
	/// <param name="fileName">テクスチャの名前</param>
	/// <returns></returns>
	uint32_t GetSrvIndex(const std::string& directoryPath, const std::string& fileName);

	/// <summary>
	/// SRVインデックスの取得
	/// </summary>
	/// <param name="filePath">Engine/resource~~テクスチャの名前までの全てのパス</param>
	/// <returns></returns>
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);
	/// <summary>
	/// テクスチャの名前からGPUハンドルを取得
	/// </summary>
	/// <param name="directoryPath">テクスチャの入っているフォルダの名前(Engine/resources/は書く必要なし)</param>
	/// <param name="fileName">テクスチャの名前</param>
	/// <returns></returns>
	uint32_t GetTextureIndexByFilePath(const std::string& directoryPath, const std::string& fileName);

	// textureResource(ユーザー使用禁止)
	Microsoft::WRL::ComPtr<ID3D12Resource> GetTextureResource(const std::string& directoryPath, const std::string& fileName) {
		TextureData& textureData = textureDatas_["Engine/resources/" + directoryPath + "/" + fileName];
		return textureData.resource;
	}

private:
	// DirectX12のTextureResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	// 中間リソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	// TextureResourceにデータを転送する
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);

public:// 定数
	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

private:
	SrvManager* srvManager_;
	// テクスチャデータ
	std::unordered_map<std::string, TextureData> textureDatas_;
};
