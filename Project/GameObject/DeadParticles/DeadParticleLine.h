#pragma once
#include "DeadParticle.h"
#include <array>

// パーティクル生成情報
struct EmitterData {
	WorldTransform worldTransform;	// パーティクルの生成地点
	Vector3 velocity;				// 速度
	float spawnTime;				// パーティクルを発生する時間
	bool isActive;					// 使用可能か
};

/// <summary>
/// 死亡パーティクルの移動処理
/// </summary>
class DeadParticleLine {
public:
	DeadParticleLine() = default;
	~DeadParticleLine();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera, const Vector3& velocity, const Vector3& worldPos);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(ViewProjection viewProjection);

	/// <summary>
	/// パーティクルの生成
	/// </summary>
	void CreateParticle();

private:
	/// <summary>
	/// パーティクルの更新処理
	/// </summary>
	void ParticleUpdate();

	bool ReleaseCheckParticle();

public:
	/// <summary>
	/// 全てのパーティクルを出し切り、消えたかを取得
	/// </summary>
	/// <returns></returns>
	const bool& GetIsFinished() { return isFinished_; }

	/// <summary>
	/// エミッターの生成場所の設定
	/// </summary>
	/// <param name="worldPos">ワールド座標</param>
	void SetEmitterPos(const Vector3& worldPos) { emitterData_.worldTransform.translate = worldPos; }

	/// <summary>
	/// 死亡パーティクルで使うテクスチャ配列を設定
	/// </summary>
	/// <param name="deadParticleTextures">死亡パーティクルで使うテクスチャ群</param>
	void SetDeadParticleTextures(std::vector<uint32_t> deadParticleTextures) { deadParticleTextures_ = deadParticleTextures; }

	/// <summary>
	/// エミッターからパーティクルが出るようにするかを設定
	/// </summary>
	void SetIsActive(const bool& isActive) { emitterData_.isActive = isActive; }

	void Start() {
		emitterData_.isActive = true;
		isFinished_ = false;
	}

public:
	// パーティクルを沸かせる時間
	const float kMaxSpawnTime = 60.0f;

private:
	Camera* camera_;

	// 死亡時のパーティクルの列
	std::list<DeadParticle*> deadParticle_;

	// パーティクル生成情報
	EmitterData emitterData_;

	std::vector<uint32_t> deadParticleTextures_;

	// パーティクルを出し切ったか
	bool isFinished_;
};