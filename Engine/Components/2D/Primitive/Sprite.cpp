#include "Sprite.h"
#include "ImGuiManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <cassert>

Sprite::Sprite(const std::string& directoryPath, std::string textureFilePath) {
	Initialize(directoryPath, textureFilePath);
}

Sprite* Sprite::Create(const std::string& directoryPath, std::string textureFilePath)
{
	Sprite* sprite = new Sprite(directoryPath, textureFilePath);

	return sprite;
}

void Sprite::Initialize(const std::string& directoryPath, std::string textureFilePath) {
	textureManager_ = TextureManager::GetInstance();
	psoManager_ = PipelineManager::GetInstance();

	/// メモリ確保
	// 頂点データ
	CreateVertexResource();
	CreateVertexBufferView();
	// Index
	CreateIndexResource();
	CreateIndexBufferView();
	// material
	CreateMaterialResource();

	// 1つ分のサイズを用意する
	cameraPosResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));

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

	/// 頂点座標の設定
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(directoryPath, textureFilePath);
	if (textureIndex_ != UINT32_MAX) {
		AdjustTextureSize(directoryPath, textureFilePath);
		size_ = textureSize_;
	}

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

	ID3D12Resource* textureBuffer = textureManager_->GetTextureResource(directoryPath, textureFilePath).Get();
	// 指定番号の画像が読み込み済みなら
	if (textureBuffer) {
		//// テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();
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
	}

	// アンカーポイントのスクリーン座標
	worldTransform_.Initialize();
	worldTransform_.transform.translate = { 0,0,1 };

	// カメラ
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	//viewProjection_.Initialize();
	//viewProjection_.constMap->view = MakeIdentity4x4();
	//viewProjection_.constMap->projection = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth_), float(WinApp::kClientHeight_), 0.0f, 100.0f);
	//cameraPosData_ = viewProjection_.transform.translate;
	camera_->GetViewProjection().constMap->projection = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth_), float(WinApp::kClientHeight_), 0.0f, 100.0f);
	cameraPosData_ = camera_->GetTranslate();
}

void Sprite::Draw() {
#pragma region スプライトの大きさを決める 
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
#pragma endregion

	// ワールド座標の更新
	worldTransform_.UpdateMatrix();
	/// コマンドを積む
	// 使用するPSO
	Object3dPSO::GetInstance()->SetCommand();
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	DirectXCommon::GetInstance()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	/// CBVの設定
	// worldTransform
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform_.constBuff_->GetGPUVirtualAddress());
	// viewProjection
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, camera_->GetViewProjection().constBuff_->GetGPUVirtualAddress());
	// カメラ位置
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraPosResource_.Get()->GetGPUVirtualAddress());

	/// DescriptorTableの設定
	// texture
	//DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, textureIndex_);
	// material
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	// ライティング
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLight::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(7, SpotLight::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());

	// 描画(DrawCall/ドローコール)。6頂点で1つのインスタンス
	DirectXCommon::GetInstance()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::Release() {

}

void Sprite::AdjustTextureSize(const std::string& directoryPath, std::string textureFilePath) {
	Microsoft::WRL::ComPtr<ID3D12Resource> textureBuffer = textureManager_->GetTextureResource(directoryPath, textureFilePath).Get();
	assert(textureBuffer);

	D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();
	textureSize_.x = static_cast<float>(resDesc.Width);
	textureSize_.y = static_cast<float>(resDesc.Height);
}

void Sprite::ImGuiAdjustParameter() {
	// ウィンドウの初期サイズを指定
	ImGui::Begin("Sprite");
	ImGui::DragFloat3("Translate", &worldTransform_.transform.translate.x, 0.5f, 0, 1280, "%.1f");
	ImGui::DragFloat3("Scale", &worldTransform_.transform.scale.x, 0.1f, -5, 5, "%.1f");
	ImGui::DragFloat3("Rotate.z", &worldTransform_.transform.rotate.x, 0.1f, -6.28f, 6.28f, "%.1f");
	ImGui::DragFloat2("AnchorPoint", &anchorPoint_.x, 0.01f, -1.0f, 1.0f, "%.2f");
	ImGui::DragFloat2("Size", &size_.x, 0.1f, 0, 1280, "%.1f");
	ImGui::CheckboxFlags("isLighting", &materialData_->enableLighting, 1);
	ImGui::End();
}

Microsoft::WRL::ComPtr<ID3D12Resource> Sprite::CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
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

void Sprite::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * 4).Get();
}

void Sprite::CreateVertexBufferView() {
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Sprite::CreateIndexResource() {
	indexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * 6).Get();
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}

void Sprite::CreateIndexBufferView() {
	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}

void Sprite::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}