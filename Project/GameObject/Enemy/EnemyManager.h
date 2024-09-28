#pragma once
#include "Audio.h"
#include "AttackAlert.h"
#include "Enemy.h"
#include "FixedTurret.h"
#include "EnemyBullet.h"
#include "Boss/Titan/Titan.h"
#include "Player.h"
#include "Camera.h"
#include "CollisionManager.h"
#include "Particles.h"
#include "Score.h"

class EnemyManager {
public:
	struct SpawnPointData {
		Vector3 distance;
		bool isActive;
	};
public:
	///
	/// Default Method
	/// 

	// コンストラクタ
	EnemyManager() = default;
	// デストラクタ
	~EnemyManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	/// <summary>
	/// パーティクルの描画
	/// </summary>
	void DrawParticle();

	///
	/// User Method
	/// 
private:
	/// <summary>
	/// ダメージパーティクルの更新処理
	/// </summary>
	/// <param name="particle"></param>
	void ParticleUpdate(Particle& particle);

	/// <summary>
	/// 発生条件
	/// </summary>
	void CheckSpawn();
	/// <summary>
	/// 敵の発生
	/// </summary>
	/// <param name="pos">発生する座標</param>
	void SpawnEnemy(Vector3 pos, Vector3 rotate, Vector3 moveSpeed);
	/// <summary>
	///	固定砲台の発生
	/// </summary>
	/// <param name="pos">発生する座標</param>
	void SpawnFixedTurret(Vector3 pos, Vector3 rotate);
	/// <summary>
	///	巨人の発生(ボス)
	/// </summary>
	/// <param name="pos">発生する座標</param>
	void SpawnTitan(Vector3 pos, Vector3 rotate);

	// カメラの後ろに敵が行ってしまったら機能停止
	bool IsObjectInOppositeDirection(const Vector3& objectPosition);
	/// <summary>
	/// 全ての敵の活性化状態を調べる
	/// </summary>
	void CheckActiveState();

	Vector2 ConvertWorld2Screen(Vector3 worldPos);

public:
	/// <summary>
	/// 弾のアドレスを登録
	/// </summary>
	/// <param name="enemyBullet">弾</param>
	void AddEnemyBullet(EnemyBullet* enemyBullet) {
		// リストに登録する
		enemyBullets_.push_back(enemyBullet);
	}

#pragma region Getter
	/// <summary>
	/// 全敵のアドレスを取得
	/// </summary>
	/// <returns></returns>
	std::list<IEnemy*> GetEnemyList() { return enemy_; }
#pragma endregion

#pragma region Setter
	/// <summary>
	/// モデルの追加
	/// </summary>
	/// <param name="model">モデル</param>
	void AddModel(Model* model) { models_.push_back(model); }

	/// <summary>
	/// カメラのアドレスを設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera) { camera_ = camera; }
	/// <summary>
	/// 衝突マネージャのアドレスを設定
	/// </summary>
	/// <param name="collisionManager">衝突マネージャ</param>
	void SetCollisionManager(CollisionManager* collisionManager) { collisionManager_ = collisionManager; }
	/// <summary>
	/// 自機のアドレスを設定
	/// </summary>
	/// <param name="player"></param>
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// 出現する場所のリストを設定
	/// </summary>
	/// <param name="spawnPoints">敵の出現する場所</param>
	void SetSpawnPoints(std::vector<LevelData::EnemyPoint> spawnPoints) { 
		spawnPoints_ = spawnPoints; 
		spawnPointDatas_.resize(spawnPoints_.size());
	}

	/// <summary>
	/// 作成した被弾パーティクルをプッシュする
	/// </summary>
	/// <param name="particle"></param>
	void SetHitParticle(Particles* particle) { 
		hitParticles_.push_back(particle);
	}

	/// <summary>
	/// カメラの進行度を設定
	/// </summary>
	/// <param name="percentage">カメラの進行度[%]</param>
	void SetRailCameraProgress(float* percentage) { railCameraProgress_ = percentage; }
	/// <summary>
	/// カメラの移動ベクトルを設定
	/// </summary>
	/// <param name="cameraMoveVel">カメラの移動ベクトル</param>
	void SetCameraMoveVel(Vector3* cameraMoveVel) { cameraMoveVel_ = cameraMoveVel; }
#pragma endregion

private:// プライベートなメンバ変数
	// 基本機能
	TextureManager* textureManager_;
	// 音
	Audio* audio_;

	// カメラのアドレス
	Camera* camera_;
	// 衝突マネージャのアドレス
	CollisionManager* collisionManager_;
	// 自機のアドレス
	Player* player_;
	// スコアクラスのインスタンス
	Score* score_;

	// 敵
	std::list<IEnemy*> enemy_;
	// 攻撃の警告
	AttackAlert* attackAlert_;
	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;
	// 出現時のパーティクル
	std::vector<Particles*> spawnParticles_;
	// 被弾時時のパーティクル
	std::vector<Particles*> hitParticles_;

	// 使用するモデル
	std::vector<Model*> models_;

	// 出現時のパーティクルのテクスチャ
	uint32_t spawnParticleTex_;
	// 被弾時のパーティクルのテクスチャ
	uint32_t hitParticleTex_;

	// 死亡SE
	uint32_t deadSE_;

	// 敵の発生する場所
	std::vector<LevelData::EnemyPoint> spawnPoints_;
	// 自機と出現場所の距離
	std::vector<SpawnPointData> spawnPointDatas_;

	// レールカメラの進行度
	float* railCameraProgress_;
	// カメラの移動ベクトル
	const Vector3* cameraMoveVel_;

	// 敵の管理番号を与える
	int id_;
};