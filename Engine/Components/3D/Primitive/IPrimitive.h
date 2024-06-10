#pragma once
#include "DirectXCommon.h"
#include "MathStructs.h"
#include "ModelStructs.h"
#include "Camera.h"
#include "WorldTransform.h"

class IPrimitive {
public:
	struct CubeVertexData {
		Vector4 position;
		Vector3 texcoord;
	};
public:
	/// 
	/// Default Method
	/// 

	/// <summary>
	/// コンストラクタ
	/// </summary>
	IPrimitive();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IPrimitive();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="textureHandle">テクスチャ番号</param>
	virtual void Draw(uint32_t textureHandle);

	/// 
	/// User Method
	/// 

	// Getter
	const Microsoft::WRL::ComPtr<ID3D12Resource> GetMaterialResource() { return materialResource_.Get(); }

	// Setter
	// 使用するカメラを設定
	void SetCamera(Camera* camera) { camera_ = camera; }

	void ImGuiAdjustParameter();

protected:
	// Resource生成
	const Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	// VertexResourceの生成
	void CreateVertexResource();

	// VertexBufferViewの生成
	void CreateVertexBufferView();

	// MaterialResourceの生成
	void CreateMaterialResource();

protected:
	// 基本機能
	DirectXCommon* dxCommon_;

	// ワールド座標
	WorldTransform worldTransform_;
	// カメラ
	Camera* camera_;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraPosResource_;
	Vector3 cameraPosData_;
	// vertex
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	CubeVertexData* vertexData_;
	// index
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	uint32_t* indexData_ = nullptr;
	// material
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_;
	Transform uvTransform_;
	Matrix4x4 uvTransformMatrix_;
};

