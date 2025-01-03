#pragma once
#include "DirectXCommon.h"
#include "MathStructs.h"
#include "ModelStructs.h"
#include "TextureManager.h"
#include "PipelineManager.h"
#include "ViewProjection.h"
#include "WinApp.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "PostEffectStructs.h"
#include <d3d12.h>
#include <wrl.h>

class TrailObject {
public:
	TrailObject() = default;
	~TrailObject() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	//void update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	// 調整項目
	void ImGuiAdjustParameter();

#pragma region Getter
	// 座標
	Vector3 GetPos() { return worldTransform_.translate; }
	// 回転角
	Vector3 GetRotate() { return worldTransform_.rotate; }
	// アドレス渡し
	Vector3* GetRotateP() { return &worldTransform_.rotate; }
	// 色取得
	Vector4 GetColor() { return materialData_->color; }
	// アドレス渡し
	Vector4* GetColorP() { return &materialData_->color; }
#pragma endregion

#pragma region Setter
	// 座標
	void SetPos(const Vector2& pos) {
		worldTransform_.translate.x = pos.x;
		worldTransform_.translate.y = pos.y;
	}
	void SetPosX(const float& x) { worldTransform_.translate.x = x; }
	void SetPosY(const float& y) { worldTransform_.translate.y = y; }
	// 回転角度を設定
	void SetRotate(Vector3& rotate) { worldTransform_.rotate = rotate; }
	// 使用するテクスチャの変更
	void SetTextureIndex(const uint32_t& textureNum) { textureNum_ = textureNum; }
	// 使用するDissolveテクスチャの変更
	void SetDissolveTextureIndex(const uint32_t& textureNum) { dissolveTextureNum_ = textureNum; }

	// dissolveの閾値を設定
	void SetDissolveMaskThreshold(const float& maskThreshold) { dissolveData_->maskThreshold = maskThreshold; }
	// dissolveを使用するかを設定
	void SetIsDissolve(const bool& isActive) { dissolveData_->isActive = isActive; }
#pragma endregion

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	// 頂点リソースの作成
	void CreateVertexResource();

	// マテリアルリソースの作成
	void CreateMaterialResource();

	// インデックスリソースの作成
	void CreateIndexResource();

	// カメラ座標リソースの作成
	void CreateCameraPosResource();

	// Dissolveリソースの作成
	void CreateDissolveResource();

public:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// uv座標
	Transform uvTransform_;

	// 描画をするか
	bool isActive_ = true;
private:
	TextureManager* textureManager_;
	PipelineManager* psoManager_;

	// カメラ
	std::unique_ptr<Camera> camera_;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraPosResource_;
	Vector3 cameraPosData_;

	// Material
	Material* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Matrix4x4 uvTransformMatrix_;
	// Vertex
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_;
	// index
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	uint32_t* indexData_ = nullptr;
	// Dissolve
	DissolveDataForGPU* dissolveData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource_;

	// 使用するテクスチャ番号
	uint32_t textureNum_;
	uint32_t dissolveTextureNum_;
};