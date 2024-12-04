#pragma once
#include "Particles.h"
#include "Sprite.h"
#include "PostEffectManager.h"
#include "PlayerConfig.h"
#include "Score.h"
#include <vector>
#include <random>

class Player;
class JustEvasionSystem {
public:
	struct JustParticleData {
		std::unique_ptr<Sprite> particle;
		Vector2 pos;
		Vector2 acceleration;
		Vector2 velocity;
		Vector2 firstVelocity;
	};

public:
	JustEvasionSystem() = default;
	~JustEvasionSystem();

	/// <summary>
	/// 初期化	
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// ジャスト回避時のパーティクルの更新処理
	/// </summary>
	void JustParticleUpdate();

	/// <summary>
	/// ジャスト回避時のパーティクル生成
	/// </summary>
	void CreateJustParticle();

	// パーティクルの速度をランダムに生成
	Vector2 GenerateRandomVelocity(Vector2 range) {
		return { GenerateRandomVelocity(range.x), GenerateRandomVelocity(range.y) };
	}
	float GenerateRandomVelocity(float range) {
		std::random_device rd;
		std::mt19937 mt(rd());

		std::uniform_real_distribution<float> dist(-range, range);
		return dist(mt);
	}

	void CheckBox();

	Vector2 ConvertWorld2Screen(Vector3 worldPos);

	/// <summary>
	/// 自機のアドレスを設定
	/// </summary>
	/// <param name="player">自機のアドレス</param>
	void SetPlayer(Player* player) { player_ = player; }
	/// <summary>
	/// カメラのアドレスを設定
	/// </summary>
	/// <param name="camera">カメラのアドレス</param>
	void SetCamera(Camera* camera) { camera_ = camera; }
	/// <summary>
	/// ジャスト回避演出を行うかの設定
	/// </summary>
	void SetIsActive(bool isActive) { isActive_ = isActive; }

public:// 定数
	// ジャスト回避時のパーティクル数
	const int kMaxJustParticleNum = 10;

	// ジャスト回避時のパーティクルの速度のランダム値範囲
	const Vector2 kJustParticleVelocityRange = { 20.0f,20.0f };

private:// エンジン機能
	TextureManager* textureManager_;
	PostEffectManager* postEffectManager_;

private:
	Player* player_;
	Camera* camera_;
	Score* score_;

	// ジャスト時に出るパーティクル
	std::vector<JustParticleData> justParticles_;

	// 回避の情報
	EvasionData evasion_;

	uint32_t justParticleTexture_;

	bool isActive_ = false;
	bool isPreActive_ = false;
};