#pragma once
#include "Audio.h"
#include "BeamEnemy.h"
#include "Camera.h"
#include "CollisionManager.h"
#include "BaseEnemy.h"
#include "MoveEnemy.h"
#include "FixedTurret.h"
#include "EnemyBullet.h"
#include "Particles.h"
#include "Player.h"
#include "RailCamera.h"
#include "Score.h"
#include "DeadParicles.h"

/// <summary>
/// 敵の管理クラス
/// </summary>
class EnemyManager {
public:
	struct SpawnPointData {
		Vector3 distance;
		bool isActive;
	};
public:
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

private:
	/// <summary>
	/// 死亡パーティクルの生成
	/// </summary>
	/// <param name="enemyPos">敵の座標</param>
	void CreateDeadParticle(const Vector3& enemyPos);

	/// <summary>
	/// 発生条件
	/// </summary>
	void CheckSpawn();
	/// <summary>
	/// 敵の発生
	/// </summary>
	/// <param name="pos">発生する座標</param>
	void SpawnEnemy(Vector3 pos, Vector3 rotate, Vector3 moveSpeed, std::vector<Vector3> controlPoints);
	/// <summary>
	///	固定砲台の発生
	/// </summary>
	/// <param name="pos">発生する座標</param>
	void SpawnFixedTurret(Vector3 pos, Vector3 rotate, std::vector<Vector3> controlPoints);
	/// <summary>
	/// ビーム敵の発生
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="rotate">回転角</param>
	void SpawnBeamEnemy(Vector3 pos, Vector3 rotate, std::vector<Vector3> controlPoints);

	/// <summary>
	/// 全ての敵弾がカメラの後ろにいるかを調べる
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
	std::list<BaseEnemy*> GetEnemyList() { return enemys_; }
	std::list<BaseEnemy*>* GetEnemyList_P() { return &enemys_; }

	/// <summary>
	/// 現在存在している敵の管理番号
	/// </summary>
	/// <returns></returns>
	std::vector<int> GetIdList() { return idList_; }
	std::vector<int>* GetIdList_P() { return &idList_; }

	const ParticleTextures& GetSpawnParticleTextures() { return spawnParticleTextures_; }
	const ParticleTextures& GetHitParticleTextures() { return hitParticleTextures_; }
#pragma endregion

#pragma region Setter
	/// <summary>
	/// モデルの追加
	/// </summary>
	/// <param name="model">モデル</param>
	void AddModel(std::string modelName, Model* model) { models_.insert(std::make_pair(modelName, std::move(model))); }

	/// <summary>
	/// カメラのアドレスを設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera) { camera_ = camera; }
	/// <summary>
	/// レールカメラのアドレスを設定
	/// </summary>
	/// <param name="railCamera">レールカメラ</param>
	void SetRailCamera(RailCamera* railCamera) { railCamera_ = railCamera; }
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
	}

	/// <summary>
	/// 作成した被弾パーティクルをプッシュする
	/// </summary>
	/// <param name="particle"></param>
	void SetHitParticle(Particles* particle) {
		hitParticles_.push_back(particle);
	}
#pragma endregion

private:
	// 死亡パーティクルの一コマに使うフレーム数
	const int kAnimationSpeed = 1;

	// 死亡パーティクルのアニメーションのコマ数
	const int kAnimationNum = 8 * kAnimationSpeed;

private:// プライベートなメンバ変数
	// 基本機能
	TextureManager* textureManager_;
	// 音
	Audio* audio_;

	// ビルボード用のカメラ
	std::unique_ptr<Camera> billboardCamera_;
	// カメラのアドレス
	Camera* camera_;
	// 追従カメラのアドレス
	RailCamera* railCamera_;
	// 衝突マネージャのアドレス
	CollisionManager* collisionManager_;
	// 自機のアドレス
	Player* player_;
	// スコアクラスのインスタンス
	Score* score_;

	// 敵
	std::list<BaseEnemy*> enemys_;
	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;
	// 出現時のパーティクル
	std::vector<Particles*> spawnParticles_;
	// 被弾時時のパーティクル
	std::vector<Particles*> hitParticles_;
	// 死亡パーティクル
	std::list<DeadParicles*> deadParicles_;

	// 使用するモデル
	std::map<std::string, Model*> models_;

	// 出現時のパーティクルのテクスチャ
	ParticleTextures spawnParticleTextures_;
	// 被弾時のパーティクルのテクスチャ
	ParticleTextures hitParticleTextures_;
	// 死亡時のパーティクル
	std::vector<uint32_t> deadParticleTextures_;
	// 弾のテクスチャ
	uint32_t bulletTexture_;

	// 死亡SE
	uint32_t deadSE_;

	// 登場時の移動用スプライン曲線制御点（通過点）
	std::vector<Vector3> controlPoints_;
	// 線分で描画する用の頂点リスト
	std::vector<Vector3> pointsDrawing_;
	// 移動ルートの線(デバッグ用)
	std::array<std::unique_ptr<Line>, 10> line_;

	// 敵の発生する場所
	std::vector<LevelData::EnemyPoint> spawnPoints_;

	// 管理番号リスト
	std::vector<int> idList_;

	// 敵の管理番号を与える
	int id_;
};