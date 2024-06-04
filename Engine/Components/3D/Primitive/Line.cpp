#include "Line.h"
#include "ImGuiManager.h"
#include "LinePSO.h"
#include <cassert>

void Line::Initialize() {
	CreateVertexResource();
	CreateMaterialResource();
	CreateVertexBufferView();
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	*materialData_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
}

void Line::Draw() {
	vertexData_[0] = { startPos_.x,startPos_.y ,startPos_.z ,1 };
	vertexData_[1] = { endPos_.x,endPos_.y ,endPos_.z ,1 };

	/// コマンドを積む
	// 使用するPSO
	LinePSO::GetInstance()->SetCommand();
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1,camera_->GetViewProjection().constBuff_->GetGPUVirtualAddress());
	// マテリアルCBufferの場所を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(2, 1, 0, 0);
}

void Line::Draw(ViewProjection viewProjection) {
	vertexData_[0] = { startPos_.x,startPos_.y ,startPos_.z ,1 };
	vertexData_[1] = { endPos_.x,endPos_.y ,endPos_.z ,1 };
	
	//worldTransform.UpdateMatrix();
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	LinePSO::GetInstance()->SetCommand();

	// コマンドを積む
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定

	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, viewProjection.constBuff_->GetGPUVirtualAddress());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// マテリアルCBufferの場所を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());

	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(2, 1, 0, 0);
}

void Line::ImGuiAdjustParameter() {
	ImGui::Begin("Line");
	/*if (ImGui::TreeNode("worldTransform")) {
		ImGui::DragFloat3("translation", &worldTransform.translation_.x, 0.01f, -50, 50);
		ImGui::DragFloat3("rotate", &worldTransform.rotation_.x, 0.001f, 0, 6.28f);
		ImGui::DragFloat3("scale", &worldTransform.scale_.x, 0.001f, 0, 10);
		ImGui::TreePop();
	}*/

	ImGui::End();
}

Microsoft::WRL::ComPtr<ID3D12Resource> Line::CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
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

void Line::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Vector4) * 2).Get();
}

void Line::CreateVertexBufferView() {
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 2;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(Vector4);
}

void Line::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Vector4)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}