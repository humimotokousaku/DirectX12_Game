#pragma once
#include "MathStructs.h"
#include "ModelStructs.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "PostEffectManager.h"
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

	// アニメーションの更新処理
	void AnimationUpdate(float& animationTime);

#pragma region Getter
	// モデルデータの取得
	ModelData GetModelData() { return modelData_; }

	// アニメーションの情報を取得
	Motion GetAnimation() { return animation_; }

	// スケルトンの情報を取得
	Skeleton GetSkeleton() { return skeleton_; }

	// スキンクラスタの情報を取得
	SkinCluster GetSkinCluster() { return skinCluster_; }

	// マテリアルの情報を取得
	Material* GetMaterialData() { return materialData_; }

	// アニメーションの行列を取得
	Matrix4x4 GetAnimationMatrix() { return animationLocalMatrix_; }
#pragma endregion

#pragma region Setter
	// アニメーションの情報を設定
	void SetAnimation(Motion animation) { animation_ = animation; }

	// スケルトンの情報を設定
	void SetSkeleton(Skeleton skeleton) { skeleton_ = skeleton; }

	// スキンクラスタの情報を設定
	void SetSkinCluster(SkinCluster skinCluster) { skinCluster_ = skinCluster; }

	// アニメーションのループ設定
	void SetIsAnimationLoop(bool isActive) { animation_.isLoop = isActive; }
	// アニメーションを行うかを設定
	void SetIsAnimation(bool isActive) { animation_.isActive = isActive; }
	// アニメーションの名前を設定
	void SetAnimationName(const char* name) { animation_.name = name; }
	/// <summary>
	/// アニメーションの再生速度
	/// 2:二倍速
	/// -:逆再生
	/// </summary>
	/// <param name="speed">再生倍率</param>
	void SetAnimationSpeed(float speed) { animation_.playBackSpeed = speed; }

	// 色の設定
	void SetColor(const Vector4& RGBA) { materialData_->color = RGBA; }

	// ライティングの設定
	void SetIsLighting(const bool& isActive) { materialData_->enableLighting = isActive; }
	// 鏡面反射の輝度の設定
	void SetShininess(const float& shininess) { materialData_->shininess = shininess; }

	// モデルに使用するテクスチャを設定
	void SetTexture(uint32_t texture) { texHandle_ = texture; }
	// dissolve用のテクスチャを設定
	void SetDissolveTexture(uint32_t dissolveTexture) { dissolveTexHandle_ = dissolveTexture; }

	// マテリアルのリソースを設定(ユーザーの使用禁止)
	void SetMaterialResource(Microsoft::WRL::ComPtr<ID3D12Resource> materialResource) { materialResource_ = materialResource; }
	// マテリアルの情報を設定(ユーザーの使用禁止)
	void SetMaterialData(Material* materialData) { materialData_ = materialData; }
	// dissolveのリソースを設定(ユーザーの使用禁止)
	void SetDissolveResource(Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource) { dissolveResource_ = dissolveResource; }
	// dissolveの情報を設定(ユーザーの使用禁止)
	void SetDissolveData(DissolveDataForGPU* dissolveData) { dissolveData_ = dissolveData; }
#pragma endregion

private:
	// スケルトンに対してアニメーションを適用
	void ApplyAnimation(float animationTime);

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
	// UVTransform
	Transform uvTransform;

private:// エンジン機能
	// DirectX
	DirectXCommon* dxCommon_;
	// テクスチャマネージャ
	TextureManager* texManager_;

private:// プライベートなメンバ変数
	// マテリアル
	Material* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	// 頂点データ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_;
	// インデックス
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	uint32_t* mappedIndex_;

	// Dissolve
	DissolveDataForGPU* dissolveData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource_;

	// カメラの座標(PSに転送用)
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraPosResource_;
	Vector3 cameraPosData_;

	// モデルデータ
	ModelData modelData_;

	// アニメーション
	Motion animation_;

	// スケルトン
	Skeleton skeleton_;

	// スキンクラスタ
	SkinCluster skinCluster_;

	Matrix4x4 uvTransformMatrix;
	Matrix4x4 animationLocalMatrix_;
	Matrix4x4 skeletonSpaceMatrix;

	// テクスチャ
	uint32_t texHandle_;
	uint32_t dissolveTexHandle_;
	uint32_t environmentTexHandle_;
};

