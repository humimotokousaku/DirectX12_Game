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
#include <d3d12.h>
#include <wrl.h>

class Sprite
{
public:
	Sprite() = default;
	~Sprite() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="size">画像の縦幅、横幅</param>
	void Initialize(const std::string& directoryPath, std::string textureFilePath);

	//void update();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="size">縦幅、横幅を入力</param>
	/// <param name="textureNum">textureManagerで登録したenum型の番号を入れる</param>
	void Draw();

	// 解放処理
	void Release();

	/// 
	/// User Method
	/// 
	// 調整項目
	void ImGuiAdjustParameter();

	/// Getter
	// 縦幅、横幅
	Vector2 GetSize() { return size_; }
	// アドレス渡し
	Vector2* GetSizeP() { return &size_; }
	// 読みこんだ時のテクスチャの大きさ
	Vector2 GetStartingSize() { return startingSize_; }
	// 座標
	Vector2 GetPos() { return { worldTransform_.translate.x, worldTransform_.translate.y }; }
	// 回転角
	Vector3 GetRotate() { return worldTransform_.rotate; }
	// アドレス渡し
	Vector3* GetRotateP() { return &worldTransform_.rotate; }
	// Spriteのアンカーポイント
	Vector2 GetAnchorPoint() { return anchorPoint_; }
	// アドレス渡し
	Vector2* GetAnchorPointP() { return &anchorPoint_; }
	// 画像の切り出しサイズ
	Vector2 GetTextureSize() { return textureSize_; }
	// アドレス渡し
	Vector2* GetTextureSizeP() { return &textureSize_; }

	/// Setter
	// 縦幅、横幅
	void SetSize(Vector2 size) { size_ = size; }
	// 座標
	void SetPos(Vector2 pos) {
		worldTransform_.translate.x = pos.x;
		worldTransform_.translate.y = pos.y;
	}
	void SetPosX(float x) {
		worldTransform_.translate.x = x;
	}
	void SetPosY(float y) {
		worldTransform_.translate.y = y;
	}
	// 回転角度を設定
	void SetRotate(Vector3 rotate) { worldTransform_.rotate = rotate; }
	// Spriteのアンカーポイント
	void SetAnchorPoint(Vector2 anchorPoint) { anchorPoint_ = anchorPoint; }
	// 画像の切り出しサイズ
	void SetTextureSize(Vector2 textureSize) { textureSize_ = textureSize; }

	// 画像の切りだしサイズのリセット
	void ResetTextureSize() { textureSize_ = { 512.0f,512.0f }; }

	// テクスチャのサイズをスプライトに合わせる
	void AdjustTextureSize(const std::string& directoryPath, std::string textureFilePath);

	// このスプライトを背景として扱う
	void SetIsBackGround() { worldTransform_.translate.z = 100000; }

	// 色を設定
	void SetColor(Vector4 color) { materialData_->color = color; }

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	void CreateVertexResource();

	void CreateVertexBufferView();

	void CreateIndexResource();

	void CreateIndexBufferView();

	void CreateMaterialResource();

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
	Transform uvTransform_;
	Matrix4x4 uvTransformMatrix_;
	// Vertex
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_;
	// index
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	uint32_t* indexData_ = nullptr;
	// Sprite
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

	// 画像の中心点
	Vector2 anchorPoint_ = { 0.5f,0.5f };

	// スプライトの縦幅、横幅
	Vector2 size_;
	// 読みこんだテクスチャの初期の大きさ
	Vector2 startingSize_;

	// 画像の左上の座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャの切り出しサイズ
	Vector2 textureSize_;

	uint32_t textureIndex_;
};

