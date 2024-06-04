#pragma once
#include "MathStructs.h"
#include "ModelStructs.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Line.h"
#include "Sphere.h"
#include <d3d12.h>

class Model
{
public:
	Model() = default;
	~Model();

	void Initialize(const std::string& directoryPath, const std::string& filename);
	void Initialize(const std::string& filename);

	void Draw(const ViewProjection& viewProjection, uint32_t textureHandle);
	void Draw(const ViewProjection& viewProjection);

	void AdjustParameter();

	///
	/// User Method
	/// 

	/// Getter
	// モデルデータ
	ModelData GetModelData() { return modelData_; }
	// アニメーションの行列
	Matrix4x4 GetAnimationMatrix() { return animationLocalMatrix_; }

	/// Setter
	void SetIsLighting(bool isActive) { materialData_->enableLighting = isActive; }

private:
	// スケルトンの線描画をするための初期化
	void SkeletonLineInit();
	// jointを描画するための初期化
	void JointSphereInit();

	//Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	void CreateVertexResource();

	void CreateVertexBufferView();

	void CreateMaterialResource();

	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	ModelData LoadModelFile(const std::string& filename);

public:
	// Material
	Material* materialData_;
	Matrix4x4 animationLocalMatrix_;
	Matrix4x4 skeletonSpaceMatrix;

	// アニメーション
	Motion animation_;
	// スケルトン
	Skeleton skeleton_;
	// スキンクラスタ
	SkinCluster skinCluster_;
private:
#pragma region エンジン機能
	// DirectX
	DirectXCommon* dxCommon_;
	// テクスチャマネージャ
	TextureManager* texManager_;
#pragma endregion

	// マテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Transform uvTransform_;
	Matrix4x4 uvTransformMatrix_;

	// テクスチャ
	uint32_t texHandle_;

	// 頂点データ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_;
	// インデックス
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	uint32_t* mappedIndex_;

	// モデルデータ
	ModelData modelData_;

	//float animationTime_ = 0.0f;
	// スケルトン描画用の線
	std::vector<Line*> skeletonLine_;
	// joint描画用の球
	std::vector<Sphere*> jointSphere_;
	

	// カメラの座標
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraPosResource_;
	Vector3 cameraPosData_;
};

