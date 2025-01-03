#include "TrailObject.h"
#include "ImGuiManager.h"
#include "PointLight.h"
#include <cassert>

void TrailObject::Initialize() {
	textureManager_ = TextureManager::GetInstance();
	psoManager_ = PipelineManager::GetInstance();

	// 頂点データのリソース作成
	CreateVertexResource();
	// インデックスリソース作成
	CreateIndexResource();
	// マテリアルデータのリソース作成
	CreateMaterialResource();
	// カメラ座標のリソース作成
	CreateCameraPosResource();
	// Dissolveリソース作成
	CreateDissolveResource();

	// アンカーポイントのスクリーン座標
	worldTransform_.Initialize();
	worldTransform_.translate = { 0,0,1 };

	// カメラ
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
}

void TrailObject::Draw() {
	// 描画しないなら早期リターン
	if (!isActive_) { return; }

#pragma region UV座標
	uvTransformMatrix_ = MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrix_ = Multiply(uvTransformMatrix_, MakeRotateZMatrix(uvTransform_.rotate.z));
	uvTransformMatrix_ = Multiply(uvTransformMatrix_, MakeTranslateMatrix(uvTransform_.translate));
	materialData_->uvTransform = uvTransformMatrix_;
#pragma endregion

	// ワールド座標の更新
	worldTransform_.UpdateMatrix();
	/// コマンドを積む
	// 使用するPSO
	PipelineManager::GetInstance()->SetObject3dPSO(kFillModeSolid);
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定

	// material
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	// worldTransform
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform_.constBuff_->GetGPUVirtualAddress());
	// texture
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, textureNum_);
	// カメラの座標
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraPosResource_.Get()->GetGPUVirtualAddress());
	// viewProjection
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, camera_->GetViewProjection().constBuff_->GetGPUVirtualAddress());
	// 点光源
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	// dissolveの情報
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, dissolveResource_->GetGPUVirtualAddress());
	// dissolve用
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(7, dissolveTextureNum_);
	// 環境テクスチャ用
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(8, textureNum_);

	// 描画(DrawCall/ドローコール)。6頂点で1つのインスタンス
	//DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(vertexIndex, 1, 0, 0);
}

void TrailObject::ImGuiAdjustParameter() {
	// ウィンドウの初期サイズを指定
	ImGui::Begin("TrailObject");
	ImGui::DragFloat3("Translate", &worldTransform_.translate.x, 0.5f, 0, 1280, "%.1f");
	ImGui::DragFloat3("Scale", &worldTransform_.scale.x, 0.01f, -5, 5, "%.1f");
	ImGui::DragFloat3("Rotate.z", &worldTransform_.rotate.x, 0.01f, -6.28f, 6.28f, "%.1f");
	ImGui::DragFloat3("UvTranslate", &uvTransform_.translate.x, 0.1f, -1000.0f, 1000.0f, "%.2f");
	ImGui::DragFloat3("UvScale", &uvTransform_.scale.x, 0.1f, -1000.0f, 1000.0f, "%.2f");
	ImGui::CheckboxFlags("isLighting", &materialData_->enableLighting, 1);
	ImGui::End();
}

Microsoft::WRL::ComPtr<ID3D12Resource> TrailObject::CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
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

void TrailObject::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * 4).Get();
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void TrailObject::CreateIndexResource() {
	indexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * 6).Get();
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}

void TrailObject::CreateCameraPosResource() {
	// 1つ分のサイズを用意する
	cameraPosResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));
}

void TrailObject::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

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
}

void TrailObject::CreateDissolveResource() {
	// Dissolveの情報を書き込む
	dissolveResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(DissolveDataForGPU)).Get();
	// 書き込むためのアドレスを取得
	dissolveResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&dissolveData_));
	dissolveData_->isActive = false;
	dissolveData_->maskThreshold = 0.5f;
}
