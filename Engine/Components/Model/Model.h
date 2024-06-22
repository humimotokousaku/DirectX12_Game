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
	void Initialize();
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
	// ライティングの設定
	void SetIsLighting(bool isActive) { materialData_->enableLighting = isActive; }
	// 鏡面反射の輝度の設定
	void SetShininess(float shininess) { materialData_->shininess = shininess; }

	// 色の設定
	void SetColor(Vector4 RGBA) { materialData_->color = RGBA; }

private:
	void CreateVertexResource();

	void CreateVertexBufferView();

	void CreateMaterialResource();

	/// <summary>
	/// ファイルの読み込み
	/// </summary>
	/// <param name="directoryPath">ファイル名よりも前のパス(Engine/resources/は書かない)</param>
	/// <param name="filename">ファイル名</param>
	/// <returns></returns>
	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	/// <summary>
	/// ファイルの読み込み
	/// </summary>
	/// <param name="fullPath">Engine/resources/よりも後の全てのパス</param>
	/// <returns></returns>
	ModelData LoadModelFile(const std::string& fullPath);

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

	// カメラの座標(PSに転送用)
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraPosResource_;
	Vector3 cameraPosData_;
};

