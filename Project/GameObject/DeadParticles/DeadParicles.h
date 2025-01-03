#pragma once
#include "DeadParticleLine.h"
#include "DeadParticle.h"
#include <random>

class DeadParicles {
public:
	DeadParicles() = default;
	~DeadParicles();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera, const Vector3& worldPos);
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
	void CreateParticle(const Vector3& worldPos);

private:
	bool ReleaseCheckParticle();

	// ランダム値を生成
	Vector3 GenerateRandomValue(Vector3 range) {
		return { GenerateRandomValue(range.x), GenerateRandomValue(range.y), GenerateRandomValue(range.z) };
	}
	float GenerateRandomValue(float range) {
		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<float> dist(-range, range);
		return dist(mt);
	}
	float GenerateRandomValue(float min, float max) {
		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<float> dist(min, max);
		return dist(mt);
	}

public:	
	/// <summary>
	/// パーティクルをすべて削除されたかを取得
	/// </summary>
	/// <returns></returns>
	const bool& GetIsFinished() { return isFinished_; }

	/// <summary>
	/// 管理番号を取得
	/// </summary>
	/// <returns></returns>
	const int& GetId() { return id_; }

	/// <summary>
	/// 死亡パーティクルで使うテクスチャ配列を設定
	/// </summary>
	/// <param name="deadParticleTextures">死亡パーティクルで使うテクスチャ群</param>
	void SetDeadParticleTextures(std::vector<uint32_t> deadParticleTextures) { deadParticleTextures_ = deadParticleTextures; }

	/// <summary>
	/// パーティクルを出すかを設定
	/// </summary>
	void SetIsActive(const bool& isActive) { isActive_ = isActive; }

	/// <summary>
	/// 管理番号を設定
	/// </summary>
	/// <param name="id">管理番号</param>
	void SetId(const int& id) { id_ = id; }

private:// 定数
	// 爆発パーティクルの大きさ
	const Vector3 kBombParticlesScale = { 3.0f,3.0f,3.0f };
	// 爆発パーティクルの移動速度
	const Vector3 kBombParticlesVelocity = { 0.0f, 2.0f, 0.0f };
	// 爆発パーティクルの数
	const int kBombParticlesNum = 2;

private:
	Camera* camera_;

	// 死亡時のパーティクルの列
	std::list<DeadParticleLine*> deadParticleLine_;
	// 爆発パーティクル
	std::list<DeadParticle*> deadBombParticle_;

	// 死亡パーティクルに使用するテクスチャ群
	std::vector<uint32_t> deadParticleTextures_;

	// パーティクル生成情報
	EmitterData emitterData_;

	int id_ = -1;

	bool isActive_;
	bool isFinished_;
};