#include "Plane.h"
#include "ImGuiManager.h"
#include "PipelineManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <cassert>
#define _USE_MATH_DEFINES
#include <math.h>

void Plane::Initialize() {
	CreateVertexResource();

	CreateMaterialResource();

	CreateWvpResource();

	CreateVertexBufferView();

	CreateIndexResource();
	CreateIndexBufferView();

	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	// 矩形のデータ
	vertexData_[0].position = { 0.0f,1.0f, 0.0f, 1.0f };// 左下
	vertexData_[1].position = { 0.0f,0.0f, 0.0f, 1.0f };// 左上
	vertexData_[2].position = { 1.0f,1.0f, 0.0f, 1.0f };// 右下
	vertexData_[3].position = { 1.0f,0.0f, 0.0f, 1.0f };// 右上
	// Index
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;

	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	// Lightingするか
	materialData_->enableLighting = true;
	// uvTransform行列の初期化
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 20;
}

void Plane::Draw(uint32_t textureHandle, const WorldTransform& worldTransform, const ViewProjection& viewProjection) {
	uvTransformMatrix_ = MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrix_ = Multiply(uvTransformMatrix_, MakeRotateZMatrix(uvTransform_.rotate.z));
	uvTransformMatrix_ = Multiply(uvTransformMatrix_, MakeTranslateMatrix(uvTransform_.translate));
	materialData_->uvTransform = uvTransformMatrix_;

	cameraPosData_ = viewProjection.translate;

	// 使用するPSO
	Object3dPSO::GetInstance()->SetCommand();

	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	DirectXCommon::GetInstance()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraPosResource_.Get()->GetGPUVirtualAddress());

	// DescriptorTableの設定
	//DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(1));
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, textureHandle);

	// マテリアルCBufferの場所を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLight::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(7, SpotLight::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());

	DirectXCommon::GetInstance()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

//void Plane::Release() {
//
//}

void Plane::ImGuiAdjustParameter() {
	ImGui::CheckboxFlags("isLighting", &materialData_->enableLighting, 1);
	ImGui::Text("UvTransform");
	ImGui::SliderFloat2("UvTranslate", &uvTransform_.translate.x, -5, 5);
	ImGui::SliderFloat2("UvScale", &uvTransform_.scale.x, -5, 5);
	ImGui::SliderAngle("UvRotate.z", &uvTransform_.rotate.z);
	ImGui::DragFloat("shininess", &materialData_->shininess, 0.01f, 0, 50);
}

Microsoft::WRL::ComPtr<ID3D12Resource> Plane::CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
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
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

void Plane::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * 4).Get();
}

void Plane::CreateVertexBufferView() {
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Plane::CreateIndexResource() {
	indexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * 6).Get();
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}

void Plane::CreateIndexBufferView() {
	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}

void Plane::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

void Plane::CreateWvpResource() {
	// 1つ分のサイズを用意する
	cameraPosResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));
}