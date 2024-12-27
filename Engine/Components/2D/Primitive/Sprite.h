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

class Sprite
{
public:
	Sprite() = default;
	~Sprite() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="textureFilePath"></param>
	void Initialize(const std::string& directoryPath, std::string textureFilePath);
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureNum">使用するテクスチャ番号</param>
	void Initialize(uint32_t textureNum);

	//void update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// 
	/// User Method
	/// 
	// 調整項目
	void ImGuiAdjustParameter();

#pragma region Getter
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
	// 色取得
	Vector4 GetColor() { return materialData_->color; }
	// アドレス渡し
	Vector4* GetColorP() { return &materialData_->color; }
#pragma endregion

#pragma region Setter
	// 縦幅、横幅
	void SetSize(Vector2 size) { size_ = size; }
	void SetSizeX(float sizeX) { size_.x = sizeX; }
	void SetSizeY(float sizeY) { size_.y = sizeY; }
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
	// 使用するテクスチャの変更
	void SetTextureIndex(uint32_t textureNum) { textureNum_ = textureNum; }
	// 使用するDissolveテクスチャの変更
	void SetDissolveTextureIndex(uint32_t textureNum) { dissolveTextureNum_ = textureNum; }

	// 画像の切りだしサイズのリセット
	void ResetTextureSize() { textureSize_ = { 512.0f,512.0f }; }

	// テクスチャのサイズをスプライトに合わせる
	void AdjustTextureSize(const std::string& directoryPath, std::string textureFilePath);
	void AdjustTextureSize(uint32_t textureNum);

	// このスプライトを背景として扱う
	void SetIsBackGround() { worldTransform_.translate.z = 100000; }

	// 色を設定
	void SetColor(Vector4 color) { materialData_->color = color; }
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

	// 使用するテクスチャ番号
	uint32_t textureNum_;
	uint32_t dissolveTextureNum_;
};

