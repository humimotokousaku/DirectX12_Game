#pragma once
#include "MathStructs.h"
#include "ModelStructs.h"
#include "Camera.h"
#include "PipelineManager.h"
#include "SrvManager.h"
#include "PostEffectManager.h"
#include "Particles.h"
#include <list>
#include <random>
#include <functional>

class DeadParticle {
public:
	///
	/// Default Method
	///

	DeadParticle();
	~DeadParticle() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="emitterPos">生成場所</param>
	/// <param name="velocity">速度</param>
	void Initialize(const Vector3& emitterPos, const Vector3& velocity);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection viewProjection);

#pragma region Getter
	/// <summary>
	/// 処理可能状態にするかを取得
	/// </summary>
	const bool& GetIsActive() { return isActive_; }

	/// <summary>
	/// パーティクルのアニメーションが終了しているかを取得
	/// </summary>
	/// <returns></returns>
	const bool& GetIsFinished() { return isFinished_; }
#pragma endregion

#pragma region Setter	
	/// <summary>
	/// 処理可能状態にするかを設定
	/// </summary>
	void SetIsActive(const bool& isActive) { isActive_ = isActive; }

	/// <summary>
	/// 座標の設定
	/// </summary>
	/// <param name="pos">座標</param>
	void SetWorldPosition(const Vector3& pos) { worldTransform_.translate = pos; }

	/// <summary>
	/// 大きさの設定
	/// </summary>
	/// <param name="scale">大きさ</param>
	void SetScale(const Vector3& scale) { worldTransform_.scale = scale; }

	/// <summary>
	/// 親子関係の設定
	/// </summary>
	/// <param name="parent">親オブジェクト</param>
	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

	/// <summary>
	/// カメラのアドレスを設定
	/// </summary>
	/// <param name="camera">カメラのアドレス</param>
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// パーティクルの消え方をイージング関数で設定
	/// </summary>
	/// <param name="easeFunc"></param>
	void SetParticleFade(std::function<float(float)> easeFunc) { easeFunc_ = easeFunc; }

	/// <summary>
	/// Dissolveの閾値を設定
	/// </summary>
	/// <param name="maskThreshold">閾値</param>
	void SetDissolveMaskThreshold(const float& maskThreshold) { dissolveData_->maskThreshold = maskThreshold; }
	/// <summary>
	/// Dissolveを使用するかを設定
	/// </summary>
	/// <param name="isActive">使用するか</param>
	void SetIsDissolve(const bool& isActive) { dissolveData_->isActive = isActive; }

	/// <summary>
	/// パーティクルに使用するテクスチャの設定
	/// </summary>
	/// <param name="textureNum">パーティクルに使用するTexture</param>
	void SetTexture(const uint32_t& textureNum) { textures_.particle = textureNum; }
	/// <summary>
	/// Dissolve用のテクスチャの設定
	/// </summary>
	/// <param name="textureNum">Dissolveに使用するTexture</param>
	void SetDissolveTexture(const uint32_t& textureNum) { textures_.dissolve = textureNum; }
	/// <summary>
	/// パーティクルに使用するテクスチャ群の設定
	/// </summary>
	/// <param name="textureNum">パーティクルに使用するTexture</param>
	void SetTextures(const ParticleTextures& textureNums) { textures_ = textureNums; }
	/// <summary>
	/// 死亡パーティクルで使うテクスチャ配列を設定
	/// </summary>
	/// <param name="deadParticleTextures">死亡パーティクルで使うテクスチャ群</param>
	void SetDeadParticleTextures(std::vector<uint32_t> deadParticleTextures) { deadParticleTextures_ = deadParticleTextures; }
#pragma endregion

private:
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

public:// 定数
	// 一コマに使うフレーム数
	const int kAnimationSpeed = 1;

	// アニメーションのコマ数
	const int kAnimationNum = 8 * kAnimationSpeed;

private:// エンジン機能
	TextureManager* textureManager_;
	DirectXCommon* dxCommon_;

private:
	// パーティクルの消え方で使用するイージング
	std::function<float(float)> easeFunc_;

	// 死亡時のパーティクル画像群
	std::vector<uint32_t> deadParticleTextures_;
	// パーティクルで使用するテクスチャ群
	ParticleTextures textures_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// 速度
	Vector3 velocity_;

	// 生存時間
	float lifeTime_;

	bool isActive_;

	// パーティクルのアニメーションが終了しているか
	bool isFinished_;

	// カメラ
	Camera* camera_;
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
	// Dissolve
	DissolveDataForGPU* dissolveData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource_;
};