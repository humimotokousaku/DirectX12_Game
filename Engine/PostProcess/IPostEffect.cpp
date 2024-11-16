#include "IPostEffect.h"

void IPostEffect::Initialize() {
	// 基本機能
	directXCommon_ = DirectXCommon::GetInstance();

	HRESULT result;

#pragma region DSV
	// 深度バッファリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			WinApp::kClientWidth_,
			WinApp::kClientHeight_,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
	// 深度バッファの生成
	D3D12_HEAP_PROPERTIES depthHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0F, 0);
	result = directXCommon_->GetDevice()->CreateCommittedResource(
		&depthHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&depthTexBuff_.resource)
	);
	assert(SUCCEEDED(result));

	// DSV用のデスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC DSVHeapDesc{};
	DSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSVHeapDesc.NumDescriptors = 1;
	// DSV用デスクリプタヒープを作成
	result = directXCommon_->GetDevice()->CreateDescriptorHeap(&DSVHeapDesc, IID_PPV_ARGS(&descHeapDSV_));
	assert(SUCCEEDED(result));
	// デスクリプタヒープにDSV作成
	const uint32_t descriptorSizeRTV = directXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	directXCommon_->GetDevice()->CreateDepthStencilView(
		depthTexBuff_.resource.Get(),
		&dsvDesc,
		GetCPUDescriptorHandle(descHeapDSV_.Get(), descriptorSizeRTV, 0)
	);
#pragma endregion

	// テクスチャバッファの作成
	texBuff_.resource = CreateTextureBufferResource();
	//depthTexBuff_.resource = depthBuff_;

#pragma region SRV
	// ノーマル
	texBuff_ = CreateSRV(texBuff_);
	depthTexBuff_ = CreateDepthSRV(depthTexBuff_);

#pragma endregion

#pragma region RTV
	// RTV用のデスクリプタヒープ
	descHeapRTV_ = CreateRTVDescriptorHeap(directXCommon_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false).Get();

	// RTVの作成
	// 何も加工しない
	CreateRTV(texBuff_, 0);
	//CreateRTV(depthTexBuff_, 0);
#pragma endregion

	// スプライトの初期化
	SpriteInitialize(texBuff_);
}

void IPostEffect::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	// ワールド座標の更新
	worldTransform_.UpdateMatrix();

	/// コマンドを積む
	PipelineManager::GetInstance()->SetPostEffectPSO(psoNum, resource);

	// VBVを設定
	directXCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	directXCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	// material
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	// texture
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(1, texBuff_.srvIndex);
	// worldTransform
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(2, worldTransform_.constBuff_->GetGPUVirtualAddress());
	// viewProjection
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, camera_->GetViewProjection().constBuff_->GetGPUVirtualAddress());

	// 描画(DrawCall/ドローコール)。6頂点で1つのインスタンス
	directXCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void IPostEffect::PreDrawScene() {
	// バリアを張る
	SetBarrier(texBuff_.resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, 1);

	// 深度ステンシルビューのデスクリプタヒープのハンドルを取得
	const uint32_t descriptorSizeRTV = directXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetCPUDescriptorHandle(descHeapDSV_.Get(), descriptorSizeRTV, 0);
	// レンダーターゲットをセット
	directXCommon_->GetCommandList()->OMSetRenderTargets(1, &rtvHandles_[0], false, &dsvHandle);

	// ビューポートの設定
	D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (FLOAT)WinApp::kClientWidth_, (FLOAT)WinApp::kClientHeight_);
	directXCommon_->GetCommandList()->RSSetViewports(1, &viewport);
	// シザー矩形の設定
	D3D12_RECT scissor = CD3DX12_RECT(0, 0, WinApp::kClientWidth_, WinApp::kClientHeight_);
	directXCommon_->GetCommandList()->RSSetScissorRects(1, &scissor);

	// 全画面クリア
	directXCommon_->GetCommandList()->ClearRenderTargetView(rtvHandles_[0], clearColor_, 0, nullptr);
	// 深度バッファのクリア
	directXCommon_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void IPostEffect::PostDrawScene() {
	// バリアの変更
	SetBarrier(texBuff_.resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 1);
}

void IPostEffect::SpriteInitialize(RenderingTextureData texData) {
	/// メモリ確保
	// 頂点データ
	CreateVertexResource();
	CreateVertexBufferView();
	// Index
	CreateIndexResource();
	CreateIndexBufferView();
	// material
	CreateMaterialResource();

	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	/// uvの設定
	// 色
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	// Lightingするか
	materialData_->enableLighting = false;
	// uvTransform行列の初期化
	materialData_->uvTransform = MakeIdentity4x4();
	// uvを動かすための座標
	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	// サイズ指定
	D3D12_RESOURCE_DESC resDesc = texData.resource->GetDesc();
	textureSize_.x = static_cast<float>(resDesc.Width);
	textureSize_.y = static_cast<float>(resDesc.Height);
	size_ = textureSize_;

	// アンカーポイントから見た頂点座標
	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;
	// 矩形のデータ
	vertexData_[0].position = { left,bottom, 0.0f, 1.0f };// 左下
	vertexData_[1].position = { left,top, 0.0f, 1.0f };// 左上
	vertexData_[2].position = { right,bottom, 0.0f, 1.0f };// 右下
	vertexData_[3].position = { right,top, 0.0f, 1.0f };// 右上
	// Index
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;

	// UVの頂点
	float tex_left = textureLeftTop_.x / resDesc.Width;
	float tex_right = (textureLeftTop_.x + textureSize_.x) / resDesc.Width;
	float tex_top = textureLeftTop_.y / resDesc.Height;
	float tex_bottom = (textureLeftTop_.y + textureSize_.y) / resDesc.Height;
	// 頂点のUVに反映
	vertexData_[0].texcoord = { tex_left, tex_bottom };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[1].texcoord = { tex_left, tex_top };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[2].texcoord = { tex_right, tex_bottom };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[3].texcoord = { tex_right, tex_top };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

	// アンカーポイントのスクリーン座標
	worldTransform_.Initialize();
	worldTransform_.translate = { 640,360,1 };

	// カメラ
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->GetViewProjection().constMap->projection = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth_), float(WinApp::kClientHeight_), 0.0f, 100.0f);
	//cameraPosData_ = camera_->GetTranslate();
}

D3D12_CPU_DESCRIPTOR_HANDLE IPostEffect::GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap.Get()->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE IPostEffect::GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap.Get()->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

#pragma region Sprite作成に必要なもの
Microsoft::WRL::ComPtr<ID3D12Resource> IPostEffect::CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
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

void IPostEffect::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * 4).Get();
}

void IPostEffect::CreateVertexBufferView() {
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void IPostEffect::CreateIndexResource() {
	indexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * 6).Get();
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}

void IPostEffect::CreateIndexBufferView() {
	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}

void IPostEffect::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}
#pragma endregion

Microsoft::WRL::ComPtr<ID3D12Resource> IPostEffect::CreateTextureBufferResource() {
	HRESULT result;
	// テクスチャリソースの設定
	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinApp::kClientWidth_,
		(UINT)WinApp::kClientHeight_,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	// テクスチャバッファの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff;
	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	D3D12_CLEAR_VALUE clearColor = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor_);
	result = directXCommon_->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearColor,
		IID_PPV_ARGS(&texBuff)
	);
	assert(SUCCEEDED(result));

	return texBuff;
}

Microsoft::WRL::ComPtr<ID3D12Resource> IPostEffect::CreateDepthTextureBufferResource() {
	HRESULT result;
	// 深度バッファリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			WinApp::kClientWidth_,
			WinApp::kClientHeight_,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
	// 深度バッファの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> texBuff;
	D3D12_HEAP_PROPERTIES depthHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0F, 0);
	result = directXCommon_->GetDevice()->CreateCommittedResource(
		&depthHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&texBuff)
	);
	assert(SUCCEEDED(result));

	// テクスチャの設定
	// 画素数(1280 x 720 = 921600)
	const UINT pixelCount = WinApp::kClientWidth_ * WinApp::kClientHeight_;
	// 画像1行分のデータサイズ
	const UINT rowPitch = sizeof(UINT) * WinApp::kClientWidth_;
	// 画像全体のデータサイズ
	const UINT depthPitch = rowPitch * WinApp::kClientHeight_;
	// 画像イメージ
	UINT* img = new UINT[pixelCount];
	for (UINT i = 0; i < pixelCount; i++) {
		img[i] = 0xff0000ff;
	}

	// テクスチャバッファにデータ転送
	result = texBuff->WriteToSubresource(
		0, nullptr,
		img,
		rowPitch,
		depthPitch
	);
	assert(SUCCEEDED(result));
	delete[] img;

	return texBuff;
}

IPostEffect::RenderingTextureData IPostEffect::CreateSRV(RenderingTextureData texData) {
	// デスクリプタヒープにSRV作成
	RenderingTextureData texBuff = texData;
	texBuff.srvIndex = SrvManager::GetInstance()->Allocate();
	texBuff.srvHandleCPU = SrvManager::GetInstance()->GetCPUDescriptorHandle(texBuff.srvIndex);
	texBuff.srvHandleGPU = SrvManager::GetInstance()->GetGPUDescriptorHandle(texBuff.srvIndex);
	SrvManager::GetInstance()->CreateSRVforPostEffect(texBuff.srvIndex, texBuff.resource.Get());

	return texBuff;
}

IPostEffect::RenderingTextureData IPostEffect::CreateDepthSRV(RenderingTextureData texData) {
	// デスクリプタヒープにSRV作成
	RenderingTextureData texBuff = texData;
	texBuff.srvIndex = SrvManager::GetInstance()->Allocate();
	texBuff.srvHandleCPU = SrvManager::GetInstance()->GetCPUDescriptorHandle(texBuff.srvIndex);
	texBuff.srvHandleGPU = SrvManager::GetInstance()->GetGPUDescriptorHandle(texBuff.srvIndex);
	SrvManager::GetInstance()->CreateSRVforDepth(texBuff.srvIndex, texBuff.resource.Get());

	return texBuff;
}

void IPostEffect::CreateRTV(RenderingTextureData texData, uint32_t index) {
	// レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	// シェーダの計算結果をSRGBに変換して書き込む
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// デスクリプタヒープにRTV作成
	const uint32_t descriptorSizeRTV = directXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	rtvHandles_[index] = GetCPUDescriptorHandle(descHeapRTV_.Get(), descriptorSizeRTV, index);
	directXCommon_->GetDevice()->CreateRenderTargetView(
		texData.resource.Get(),
		&renderTargetViewDesc,
		rtvHandles_[index]
	);
}

void IPostEffect::SetBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> texBuff, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState, UINT numBarrier) {
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff.Get(), beforeState, afterState, 0, D3D12_RESOURCE_BARRIER_FLAG_NONE);
	directXCommon_->GetCommandList()->ResourceBarrier(numBarrier, &barrier);
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> IPostEffect::CreateRTVDescriptorHeap(const Microsoft::WRL::ComPtr<ID3D12Device>& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = heapType; // レンダーターゲットビュー用
	rtvDescriptorHeapDesc.NumDescriptors = numDescriptors;
	rtvDescriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = device.Get()->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	// DiscriptorHeapが作れなかったので起動できない
	assert(SUCCEEDED(hr));

	return descriptorHeap;
}

const float IPostEffect::clearColor_[4] = { 0.0f, 0.25f, 0.5f, 1.0f };