#include "IPrimitive.h"
#include "ImGuiManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SrvManager.h"
#include "Object3DPSO.h"
#include <cassert>

IPrimitive::IPrimitive() {
}

IPrimitive::~IPrimitive() {

}

void IPrimitive::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();

	// 頂点リソース確保
	CreateVertexResource();
	// 頂点のBufferViewを確保
	CreateVertexBufferView();
	// インデックスのリソース確保
	indexResource_ = CreateBufferResource(sizeof(uint32_t) * 36).Get();
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	// インデックスのBV確保
	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 36;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	// マテリアルのリソース確保
	CreateMaterialResource();

	// カメラ
	// 1つ分のサイズを用意する
	cameraPosResource_ = CreateBufferResource(sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));

	uvTransform_ = {
	{1.0f,1.0f,1.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f}
	};

#pragma region 右面
	vertexData_[0].position = { 1.0f, 1.0f, 1.0f,1.0f };	
	vertexData_[1].position = { 1.0f, 1.0f, -1.0f ,1.0f };
	vertexData_[2].position = { 1.0f, -1.0f, 1.0f,1.0f };
	vertexData_[3].position = { 1.0f, -1.0f, -1.0f ,1.0f };

	vertexData_[0].texcoord = { 1.0f,1.0f, 1.0f };
	vertexData_[1].texcoord = { 1.0f,1.0f, -1.0f };
	vertexData_[2].texcoord = { 1.0f,-1.0f, 1.0f };
	vertexData_[3].texcoord = { 1.0f,-1.0f, -1.0f };
#pragma endregion

#pragma region 左面
	vertexData_[4].position = { -1.0f, 1.0f, -1.0f, 1.0f };
	vertexData_[5].position = { -1.0f, 1.0f, 1.0f, 1.0f };
	vertexData_[6].position = { -1.0f, -1.0f, -1.0f, 1.0f };
	vertexData_[7].position = { -1.0f, -1.0f, 1.0f, 1.0f };

	vertexData_[4].texcoord = { -1.0f,1.0f, -1.0f };
	vertexData_[5].texcoord = { -1.0f,1.0f, 1.0f };
	vertexData_[6].texcoord = { -1.0f,-1.0f, -1.0f };
	vertexData_[7].texcoord = { -1.0f,-1.0f, 1.0f };
#pragma endregion

#pragma region 前面
	vertexData_[8].position = { -1.0f, 1.0f, 1.0f ,1.0f };
	vertexData_[9].position = { 1.0f, 1.0f, 1.0f ,1.0f };
	vertexData_[10].position = { -1.0f, -1.0f, 1.0f ,1.0f };
	vertexData_[11].position = { 1.0f, -1.0f, 1.0f ,1.0f };

	vertexData_[8].texcoord = { -1.0f,1.0f, 1.0f };
	vertexData_[9].texcoord = { 1.0f,1.0f, 1.0f };
	vertexData_[10].texcoord = { -1.0f,-1.0f, 1.0f };
	vertexData_[11].texcoord = { 1.0f,-1.0f, 1.0f };
#pragma endregion

#pragma region 後面
	vertexData_[12].position = { 1.0f, 1.0f, -1.0f ,1.0f };
	vertexData_[13].position = { -1.0f, 1.0f, -1.0f ,1.0f };
	vertexData_[14].position = { 1.0f, -1.0f, -1.0f ,1.0f };
	vertexData_[15].position = { -1.0f, -1.0f, -1.0f ,1.0f };

	vertexData_[12].texcoord = { 1.0f,1.0f, -1.0f };
	vertexData_[13].texcoord = { -1.0f,1.0f, -1.0f };
	vertexData_[14].texcoord = { 1.0f,-1.0f, -1.0f };
	vertexData_[15].texcoord = { -1.0f,-1.0f, -1.0f };
#pragma endregion

#pragma region 上面
	vertexData_[16].position = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertexData_[17].position = { -1.0f, 1.0f, 1.0f, 1.0f };
	vertexData_[18].position = { 1.0f, 1.0f, -1.0f, 1.0f };
	vertexData_[19].position = { -1.0f, 1.0f, -1.0f, 1.0f };

	vertexData_[16].texcoord = { 1.0f,1.0f, 1.0f };
	vertexData_[17].texcoord = { -1.0f,1.0f, 1.0f };
	vertexData_[18].texcoord = { 1.0f,1.0f, -1.0f };
	vertexData_[19].texcoord = { -1.0f,1.0f, -1.0f };
#pragma endregion

#pragma region 底面
	vertexData_[20].position = { -1.0f, -1.0f, 1.0f, 1.0f };
	vertexData_[21].position = { 1.0f, -1.0f, 1.0f, 1.0f };
	vertexData_[22].position = { -1.0f, -1.0f, -1.0f, 1.0f };
	vertexData_[23].position = { 1.0f, -1.0f, -1.0f, 1.0f };

	vertexData_[20].texcoord = { -1.0f,-1.0f, 1.0f };
	vertexData_[21].texcoord = { 1.0f,-1.0f, 1.0f };
	vertexData_[22].texcoord = { -1.0f,-1.0f, -1.0f };
	vertexData_[23].texcoord = { 1.0f,-1.0f, -1.0f };
#pragma endregion
	// インデックス
	for (int i = 0; i < 6; i++) {
		indexData_[6 * i] = 4 * i;
		indexData_[6 * i + 1] = 4 * i + 1;
		indexData_[6 * i + 2] = 4 * i + 2;
		indexData_[6 * i + 3] = 4 * i + 2;
		indexData_[6 * i + 4] = 4 * i + 1;
		indexData_[6 * i + 5] = 4 * i + 3;
	}

	// 色
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->enableLighting = false;

	// ワールド座標
	worldTransform_.Initialize();
}

void IPrimitive::Draw(uint32_t textureHandle) {
	worldTransform_.UpdateMatrix();
	// コマンドを積む
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	DirectXCommon::GetInstance()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	// マテリアルCBufferの場所を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	// DescriptorTableの設定
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(1, textureHandle);
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(2, worldTransform_.constBuff_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, camera_->GetViewProjection().constBuff_->GetGPUVirtualAddress());

	// 描画(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	DirectXCommon::GetInstance()->GetCommandList()->DrawIndexedInstanced(36, 1, 0, 0 ,0);
}

void IPrimitive::ImGuiAdjustParameter() {

}

const Microsoft::WRL::ComPtr<ID3D12Resource> IPrimitive::CreateBufferResource(size_t sizeInBytes) {
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
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertexResource.GetAddressOf()));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

void IPrimitive::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(sizeof(CubeVertexData) * 36).Get();
	// 書き込むためのアドレスを取得
	vertexResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
}

void IPrimitive::CreateVertexBufferView() {
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(CubeVertexData) * 36;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(CubeVertexData);
}

void IPrimitive::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(sizeof(Material)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}