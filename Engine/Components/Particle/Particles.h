#pragma once
#include "MathStructs.h"
#include "ModelStructs.h"
#include "Model.h"
#include "CollisionConfig.h"
#include "Camera.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "PipelineManager.h"
#include "SrvManager.h"
#include "PostEffectManager.h"
#include <list>
#include <random>
#include <functional>

// GPUに送る
struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct Particle {
	Transform transform;	// 座標
	Vector3 vel;			// 速度
	Vector4 color;			// 色
	float lifeTime;			// 生存時間
	float currentTime = 0;	// 経過フレーム
};
// Particleを発生させる
struct Emitter {
	WorldTransform transform;
	int32_t count;
	int32_t spawnCount;
	int32_t spawnLeft = 1;	// 発生の残り回数 
	float frequency;
	float frequencyTime;
};

struct AABB {
	Vector3 min;
	Vector3 max;
};
struct AccField {
	Vector3 acc;  // 加速度
	AABB area;	  // 範囲
	bool isActive;// Fieldの活性化
};

// パーティクルに使用するテクスチャ
struct ParticleTextures {
	uint32_t particle;			// パーティクル用のテクスチャ
	uint32_t dissolve;			// dissolve用のテクスチャ
};

class Particles {
public:
	///
	/// Default Method
	///

	Particles();
	~Particles();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="emitterPos">生成場所</param>
	void Initialize(const Vector3& emitterPos);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	///
	/// User Method
	///

#pragma region Getter
	// ワールド座標を取得
	Vector3 GetEmitterWorldPosition() {
		Vector3 worldPos = {
			emitter_.transform.matWorld_.m[3][0],
			emitter_.transform.matWorld_.m[3][1],
			emitter_.transform.matWorld_.m[3][2]
		};
		return worldPos;
	}
#pragma endregion

#pragma region Setter	
	// カメラ
	void SetCamera(Camera* camera) { camera_ = camera; }
	/// <summary>
	/// パーティクルの挙動の設定
	/// </summary>
	/// <param name="updateFunc">void (Particle＆);の関数</param>
	void SetParticleUpdate(std::function<void(Particle&)> updateFunc) { updateFunc_ = updateFunc; }
	/// <summary>
	/// パーティクルの消え方をイージング関数で設定
	/// </summary>
	/// <param name="easeFunc"></param>
	void SetParticleFade(std::function<float(float)> easeFunc) { easeFunc_ = easeFunc; }

	/// <summary>
	/// エミッターのワールドトランスフォームとペアレントする
	/// </summary>
	void SetEmitterParent(const WorldTransform* parent) { emitter_.transform.parent_ = parent; }
	/// <summary>
	/// パーティクルの発生源の座標
	/// </summary>
	/// <param name="translate">座標</param>
	void SetEmitterPos(const Vector3& translate) { emitter_.transform.translate = translate; }
	/// <summary>
	/// 一回で発生するパーティクルの数
	/// </summary>
	/// <param name="count">発生するパーティクルの数</param>
	void SetEmitterCount(const uint32_t& count) { emitter_.count = count; }
	/// <summary>
	/// パーティクルの発生の残り回数を設定
	/// </summary>
	/// <param name="spawnLeft"></param>
	void SetEmitterSpawnLeft(const uint32_t& spawnLeft) { emitter_.spawnLeft = spawnLeft; }
	/// <summary>
	/// パーティクルの発生回数(0なら無制限に出る)
	/// </summary>
	/// <param name="spawnCount">発生回数</param>
	void SetEmitterSpawnCount(const uint32_t& spawnCount) { emitter_.spawnCount = spawnCount; }
	/// <summary>
	/// パーティクルが発生する頻度
	/// </summary>
	/// <param name="frequency">秒</param>
	void SetEmitterFrequency(const float& frequency) {
		emitter_.frequency = frequency;
		emitter_.frequencyTime = frequency;
	}

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
#pragma endregion

	/// <summary>
	/// ランダムの上限下限値をすべて0にする
	/// </summary>
	void OffRandom() {
		randomTranslateLimit = Limit{ 0.0f,0.0f };
		randomScaleLimit = Limit{ 0.0f,0.0f };
		for (int i = 0; i < 3; i++) {
			randomVelLimit[i] = Limit{ 0.0f,0.0f };
		}
		randomLifeTimeLimit = Limit{ 0.0f, 0.0f };
		randomColorLimit = Limit{ 0.0f,0.0f };
	}

private:
	// particleの座標と速度のランダム生成
	Particle MakeNewParticle(std::mt19937& randomEngine);
	Particle MakeNewParticle(const Vector3& translate);

	std::list<Particle> Emission(const Emitter& emitter, std::mt19937& randomEngine);

	bool IsCollision(const AABB& aabb, const Vector3& point);

	Vector3 KelvinToRGB(int kelvin);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	void CreateVertexResource();

	void CreateVertexBufferView();

	void CreateMaterialResource();

public:
	// ランダム座標の上限下限
	Limit randomTranslateLimit = { -1.0f, 1.0f };
	// ランダムスケールの上限下限
	Limit randomScaleLimit = { 0.3f, 0.5f };
	// ランダム速度の上限下限
	// [0]:x
	// [1]:y
	// [2]:z
	Limit randomVelLimit[3] = {
		{-1.0f, 1.0f },	// x
		{-1.0f, 1.0f },	// y
		{-1.0f, 1.0f }	// z
	};
	// ランダム生存時間の下限上限
	Limit randomLifeTimeLimit = { 1.0f, 3.0f };
	// ランダムカラーの上限下限
	Limit randomColorLimit = { 0.0f, 1.0f };
	// パーティクル一粒の詳細設定
	// 座標と速度は自動的にランダムが適応されている
	Particle particle_;

	// エミッタ
	Emitter emitter_;

private:// 定数
	// 1フレームで進む時間
	const float kDeltaTime = 1.0f / 60.0f;
	// パーティクルの最大数
	const static uint32_t kNumMaxInstance = 6000;

private:
	// パーティクル
	std::list<Particle> particles_;

	// パーティクルの消え方で使用するイージング
	std::function<float(float)> easeFunc_;
	// パーティクルの挙動
	std::function<void(Particle&)> updateFunc_;

	// フィールド
	AccField accField_;

	ModelData modelData_;
	// Material
	Material* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Transform uvTransform_;
	Matrix4x4 uvTransformMatrix_;
	// Vertex
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_;
	// Dissolve
	DissolveDataForGPU* dissolveData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource_;

	// 複数描画のための変数
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;
	ParticleForGPU* instancingData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	uint32_t srvIndex_;

	// カメラ
	Camera* camera_;

	// パーティクルで使用するテクスチャ群
	ParticleTextures textures_;
};

