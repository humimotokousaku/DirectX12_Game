#include "DeadParticle.h"
#include "ImGuiManager.h"
#include "GlobalVariables.h"
#include "PointLight.h"
#include "SrvManager.h"
#include <cassert>
#include <numbers>
#include <vector>

DeadParticle::DeadParticle() {
	// パーティクルのワールドトランスフォーム
	worldTransform_.Initialize();
}

void DeadParticle::Initialize(const Vector3& emitterPos, const Vector3& velocity) {
	textureManager_ = TextureManager::GetInstance();
	dxCommon_ = DirectXCommon::GetInstance();

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

	worldTransform_.translate = emitterPos;
	worldTransform_.scale *= 1.5f;
	worldTransform_.UpdateMatrix();

	// パーティクルの速度を代入
	velocity_ = velocity;

	// パーティクルの消え方
	easeFunc_ = Easings::EaseInCubic;
}

void DeadParticle::Update() {
	// アニメーションが終了しているなら処理しない
	if (isFinished_ || !isActive_) { return; }

	// 速度を加算
	worldTransform_.translate += velocity_;

	for (int i = 0; i < kAnimationNum; i++) {
		if (lifeTime_ >= i && lifeTime_ <= i + kAnimationSpeed - 1) {
			textures_.particle = deadParticleTextures_[i];
		}
	}

	// パーティクルのアニメーションが終了しているか
	if (lifeTime_ > kAnimationNum) {
		isFinished_ = true;
	}

	lifeTime_++;
}

void DeadParticle::Draw(ViewProjection viewProjection) {
	// アニメーションが終了しているなら処理しない
	if (isFinished_ || !isActive_) { return; }

	// カメラ行列
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_->GetScale(), camera_->GetRotate(), camera_->GetTranslate());
	// 板ポリを正面に向ける
	Matrix4x4 backToFrontMatrix = MakeIdentity4x4();
	// billboardMatrixを作成
	Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	// WVPとworldMatrixの計算
	Matrix4x4 worldMatrix = MakeAffineMatrix(worldTransform_.scale, billboardMatrix, worldTransform_.translate);
	worldTransform_.matWorld_ = Multiply(worldMatrix, Multiply(camera_->GetViewProjection().matView, camera_->GetViewProjection().matProjection));
	worldTransform_.TransferMatrix();
	
	// 使用するPSO
	PipelineManager::GetInstance()->SetObject3dPSO(kFillModeSolid);
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	// material
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	// worldTransform
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform_.constBuff_->GetGPUVirtualAddress());
	// texture
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, textures_.particle);
	// カメラの座標
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraPosResource_.Get()->GetGPUVirtualAddress());
	// viewProjection
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	// 点光源
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	// dissolveの情報
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, dissolveResource_->GetGPUVirtualAddress());
	// dissolve用
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(7, textures_.dissolve);
	// 環境テクスチャ用
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(8, textures_.particle);

	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

Microsoft::WRL::ComPtr<ID3D12Resource> DeadParticle::CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
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

void DeadParticle::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 4).Get();
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 矩形のデータ
	vertexData_[0] = { .position = {-1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} };// 左下
	vertexData_[1] = { .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} };// 左上
	vertexData_[2] = { .position = {1.0f,-1.0f,0.0f,1.0f}, .texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} };// 右下
	vertexData_[3] = { .position = {1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} };// 右上
}

void DeadParticle::CreateIndexResource() {
	indexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * 6).Get();
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	// Index
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;
}

void DeadParticle::CreateCameraPosResource() {
	// 1つ分のサイズを用意する
	cameraPosResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));
}

void DeadParticle::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material)).Get();
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

void DeadParticle::CreateDissolveResource() {
	// Dissolveの情報を書き込む
	dissolveResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(DissolveDataForGPU)).Get();
	// 書き込むためのアドレスを取得
	dissolveResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&dissolveData_));
	dissolveData_->isActive = false;
	dissolveData_->maskThreshold = 0.5f;
}
