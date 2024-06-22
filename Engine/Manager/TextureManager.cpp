#include "ConvertString.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "PostEffect.h"
#include <vector>
#include <cassert>
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <sstream>

uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;

	return &instance;
}

void TextureManager::Initialize(SrvManager* srvManager) {
	srvManager_ = srvManager;
	textureDatas_.reserve(DirectXCommon::kMaxSRVCount);
	// モデルを読み込んだ際にテクスチャがないなら白い画像を送る
	LoadTexture("DefaultTexture/white.png");
}

void TextureManager::Finalize() {
	textureDatas_.clear();
}

void TextureManager::ComInit() {
	CoInitializeEx(0, COINIT_MULTITHREADED);
}

void TextureManager::ComUninit() {
	CoUninitialize();
}

void TextureManager::LoadTexture(const std::string& filePath) {
	// 読み込み済みテクスチャを検索
	if (textureDatas_.contains("Engine/resources/" + filePath)) {
		return;
	}
	// 何も読み込んでいなかったら終了
	if (filePath.size() == 0) {
		return;
	}
	// テクスチャ枚数の上限チェック
	assert(srvManager_->GetIsLimit());

	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString("Engine/resources/" + filePath);
	HRESULT hr;
	if (filePathW.ends_with(L".dds")) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	// ミニマップの生成
	DirectX::ScratchImage mipImages{};
	// 圧縮フォーマットかを検出
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		mipImages = std::move(image);
	}
	else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	}
	assert(SUCCEEDED(hr));

	// 追加したテクスチャデータの参照を取得
	TextureData& textureData = textureDatas_["Engine/resources/" + filePath];
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = CreateTextureResource(textureData.metadata);
	textureData.intermediateResource = UploadTextureData(textureData.resource, mipImages);
	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);
	srvManager_->CreateSRVforTexture2D(textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format, textureData.metadata.mipLevels, textureData.metadata);
}

void TextureManager::LoadTexture(const std::string& directoryPath, const std::string& fileName) {
	std::string filePath;
	// directoryPathが空の場合
	if (directoryPath.size() == 0) {
		filePath = "Engine/resources/" + fileName;
	}
	else {
		filePath = "Engine/resources/" + directoryPath + "/" + fileName;
	}

	// 読み込み済みテクスチャを検索
	if (textureDatas_.contains(filePath)) {
		return;
	}
	// 何も読み込んでいなかったら終了
	if (fileName.size() == 0) {
		return;
	}
	// テクスチャ枚数の上限チェック
	assert(srvManager_->GetIsLimit());

	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr;
	if (filePathW.ends_with(L".dds")) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	// ミニマップの生成
	DirectX::ScratchImage mipImages{};
	// 圧縮フォーマットかを検出
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		mipImages = std::move(image);
	}
	else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	}
	assert(SUCCEEDED(hr));

	// 追加したテクスチャデータの参照を取得
	TextureData& textureData = textureDatas_[filePath];
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = CreateTextureResource(textureData.metadata);
	textureData.intermediateResource = UploadTextureData(textureData.resource, mipImages);
	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);
	srvManager_->CreateSRVforTexture2D(textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format, textureData.metadata.mipLevels, textureData.metadata);
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath) {
	// 読み込み済みテクスチャデータを検索
	if (textureDatas_.contains("Engine/resources/" + filePath)) {
		TextureData& textureData = textureDatas_["Engine/resources/" + filePath];
		return textureData.srvIndex;
	}
	assert(0);
	return 0;
}
uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& directoryPath, const std::string& fileName) {
	std::string filePath;
	// directoryPathが空の場合
	if (directoryPath.size() == 0) {
		filePath = "Engine/resources/" + fileName;
	}
	else {
		filePath = "Engine/resources/" + directoryPath + "/" + fileName;
	}

	// 読み込み済みテクスチャデータを検索
	if (textureDatas_.contains(filePath)) {
		TextureData& textureData = textureDatas_[filePath];
		return textureData.srvIndex;
	}
	assert(0);
	return 0;
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath) {
	TextureData& textureData = textureDatas_["Engine/resources/" + filePath];
	// 何も書いてないならデフォルトテクスチャの番号を返す
	if (filePath.size() == 0) {
		textureData = textureDatas_["Engine/resources/DefaultTexture/white.png"];
	}
	return textureData.srvIndex;
}
uint32_t TextureManager::GetSrvIndex(const std::string& directoryPath, const std::string& fileName) {
	std::string filePath;
	// directoryPathが空の場合
	if (directoryPath.size() == 0) {
		filePath = "Engine/resources/" + fileName;
	}
	else {
		filePath = "Engine/resources/" + directoryPath + "/" + fileName;
	}

	TextureData& textureData = textureDatas_[filePath];
	// 何も書いてないならデフォルトテクスチャの番号を返す
	if (fileName.size() == 0) {
		textureData = textureDatas_["Engine/resources/DefaultTexture/white.png"];
	}
	return textureData.srvIndex;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap.Get()->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
	HRESULT hr;
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; // リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれからは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// 実際に頂点リソースを作る
	hr = device.Get()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertexResource.GetAddressOf()));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata) {
	// metadataをもとにResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(resource.GetAddressOf())
	);
	assert(SUCCEEDED(hr));
	return resource;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(DirectXCommon::GetInstance()->GetDevice().Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), intermediateSize).Get();
	UpdateSubresources(DirectXCommon::GetInstance()->GetCommandList().Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	// textureへの転送後は利用できるように、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	DirectXCommon::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);
	return intermediateResource;
}
