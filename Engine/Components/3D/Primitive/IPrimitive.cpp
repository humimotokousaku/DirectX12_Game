#include "IPrimitive.h"
#include "ImGuiManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SrvManager.h"
#include <cassert>

IPrimitive::IPrimitive() {
}

IPrimitive::~IPrimitive() {

}

void IPrimitive::Initialize() {
	CreateVertexResource();
	CreateMaterialResource();
	CreateVertexBufferView();
	// 書き込むためのアドレスを取得
	vertexResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	// カメラ
// 1つ分のサイズを用意する
	//cameraPosResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));

	uvTransform_ = {
	{1.0f,1.0f,1.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f}
	};

#pragma region 正面

	// 左下
	vertexData_[0].position = { -0.5f, -0.5f, -0.5f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	// 左上:
	vertexData_[1].position = { -0.5f, 0.5f, -0.5f ,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	// 右下
	vertexData_[2].position = { 0.5f, -0.5f, -0.5f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };

	// 左上
	vertexData_[3].position = { -0.5f, 0.5f, -0.5f ,1.0f };
	vertexData_[3].texcoord = { 0.0f,0.0f };
	// 右上:
	vertexData_[4].position = { 0.5f, 0.5f, -0.5f ,1.0f };
	vertexData_[4].texcoord = { 1.0f,0.0f };
	// 右下
	vertexData_[5].position = { 0.5f, -0.5f, -0.5f ,1.0f };
	vertexData_[5].texcoord = { 1.0f,1.0f };

#pragma endregion

#pragma region 左面

	// 左下
	vertexData_[6].position = { -0.5f, -0.5f, 0.5f, 1.0f };
	vertexData_[6].texcoord = { 0.0f,1.0f };
	// 左上:
	vertexData_[7].position = { -0.5f, 0.5f, 0.5f, 1.0f };
	vertexData_[7].texcoord = { 0.0f,0.0f };
	// 右下
	vertexData_[8].position = { -0.5f, -0.5f, -0.5f, 1.0f };
	vertexData_[8].texcoord = { 1.0f,1.0f };

	// 左上
	vertexData_[9].position = { -0.5f, 0.5f, 0.5f, 1.0f };
	vertexData_[9].texcoord = { 0.0f,0.0f };
	// 右上:
	vertexData_[10].position = { -0.5f, 0.5f, -0.5f, 1.0f };
	vertexData_[10].texcoord = { 1.0f,0.0f };
	// 右下
	vertexData_[11].position = { -0.5f, -0.5f, -0.5f, 1.0f };
	vertexData_[11].texcoord = { 1.0f,1.0f };

#pragma endregion

#pragma region 背面

	// 左下
	vertexData_[12].position = { 0.5f, -0.5f, 0.5f ,1.0f };
	vertexData_[12].texcoord = { 0.0f,1.0f };
	// 左上:
	vertexData_[13].position = { 0.5f, 0.5f, 0.5f ,1.0f };
	vertexData_[13].texcoord = { 0.0f,0.0f };
	// 右下
	vertexData_[14].position = { -0.5f, -0.5f, 0.5f ,1.0f };
	vertexData_[14].texcoord = { 1.0f,1.0f };

	// 左上
	vertexData_[15].position = { 0.5f, 0.5f, 0.5f ,1.0f };
	vertexData_[15].texcoord = { 0.0f,0.0f };
	// 右上:
	vertexData_[16].position = { -0.5f, 0.5f, 0.5f ,1.0f };
	vertexData_[16].texcoord = { 1.0f,0.0f };
	// 右下
	vertexData_[17].position = { -0.5f, -0.5f, 0.5f ,1.0f };
	vertexData_[17].texcoord = { 1.0f,1.0f };

#pragma endregion

#pragma region 上面

	// 左下
	vertexData_[18].position = { 0.5f, 0.5f, 0.5f, 1.0f };
	vertexData_[18].texcoord = { 0.0f,0.0f };
	// 左上:
	vertexData_[19].position = { 0.5f, 0.5f, -0.5f, 1.0f };
	vertexData_[19].texcoord = { 0.0f,1.0f };
	// 右下
	vertexData_[20].position = { -0.5f, 0.5f, 0.5f, 1.0f };
	vertexData_[20].texcoord = { 1.0f,0.0f };

	// 左上
	vertexData_[21].position = { 0.5f, 0.5f, -0.5f, 1.0f };
	vertexData_[21].texcoord = { 0.0f,1.0f };
	// 右上:
	vertexData_[22].position = { -0.5f, 0.5f, -0.5f, 1.0f };
	vertexData_[22].texcoord = { 1.0f,1.0f };
	// 右下
	vertexData_[23].position = { -0.5f, 0.5f, 0.5f, 1.0f };
	vertexData_[23].texcoord = { 1.0f,0.0f };

#pragma endregion

#pragma region 右面

	// 左下
	vertexData_[24].position = { 0.5f, -0.5f, -0.5f, 1.0f };
	vertexData_[24].texcoord = { 0.0f,1.0f };
	// 左上:
	vertexData_[25].position = { 0.5f, 0.5f, -0.5f, 1.0f };
	vertexData_[25].texcoord = { 0.0f,0.0f };
	// 右下
	vertexData_[26].position = { 0.5f, -0.5f, 0.5f, 1.0f };
	vertexData_[26].texcoord = { 1.0f,1.0f };

	// 左上
	vertexData_[27].position = { 0.5f, 0.5f, -0.5f, 1.0f };
	vertexData_[27].texcoord = { 0.0f,0.0f };
	// 右上:
	vertexData_[28].position = { 0.5f, 0.5f, 0.5f, 1.0f };
	vertexData_[28].texcoord = { 1.0f,0.0f };
	// 右下
	vertexData_[29].position = { 0.5f, -0.5f, 0.5f, 1.0f };
	vertexData_[29].texcoord = { 1.0f,1.0f };

#pragma endregion

#pragma region 底面

	// 左下
	vertexData_[30].position = { 0.5f, -0.5f, -0.5f, 1.0f };
	vertexData_[30].texcoord = { 0.0f,1.0f };
	// 左上:
	vertexData_[31].position = { 0.5f, -0.5f, 0.5f, 1.0f };
	vertexData_[31].texcoord = { 0.0f,0.0f };
	// 右下
	vertexData_[32].position = { -0.5f, -0.5f, -0.5f, 1.0f };
	vertexData_[32].texcoord = { 1.0f,1.0f };

	// 左上
	vertexData_[33].position = { 0.5f, -0.5f, 0.5f, 1.0f };
	vertexData_[33].texcoord = { 0.0f,0.0f };
	// 右上:
	vertexData_[34].position = { -0.5f, -0.5f, 0.5f, 1.0f };
	vertexData_[34].texcoord = { 1.0f,0.0f };
	// 右下
	vertexData_[35].position = { -0.5f, -0.5f, -0.5f, 1.0f };
	vertexData_[35].texcoord = { 1.0f,1.0f };

#pragma endregion

	// 法線の向き
	for (int i = 0; i < 36; i++) {
		vertexData_[i].normal = { vertexData_[i].position.x,vertexData_[i].position.y,vertexData_[i].position.z };
	}

	// 色
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->enableLighting = true;

	// ワールド座標
	worldTransform_.Initialize();
}

void IPrimitive::Draw(uint32_t textureHandle) {
	// コマンドを積む
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform_.constBuff_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, camera_->GetViewProjection().constBuff_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraPosResource_.Get()->GetGPUVirtualAddress());

	// ライティング
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLight::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(7, SpotLight::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());
	// マテリアルCBufferの場所を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());

	// DescriptorTableの設定
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, textureHandle);

	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLight::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());

	// 描画(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(36, 1, 0, 0);
}

void IPrimitive::ImGuiAdjustParameter() {
	ImGui::SliderFloat3("Scale", &transform_.scale.x, -5.0f, 5.0f);
	ImGui::SliderFloat3("Rotate", &transform_.rotate.x, -6.28f, 6.28f);
	ImGui::ColorEdit3("color", &materialData_->color.x);
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
	vertexResource_ = CreateBufferResource(sizeof(VertexData) * 36).Get();
}

void IPrimitive::CreateVertexBufferView() {
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 36;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void IPrimitive::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(sizeof(Material)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}