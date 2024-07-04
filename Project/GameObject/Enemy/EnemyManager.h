#pragma once
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Player.h"
#include "Camera.h"
#include "CollisionManager.h"

class EnemyManager {
public:
	///
	/// Default Method
	/// 

	// コンストラクタ
	EnemyManager();
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
	/// 解放処理
	/// </summary>
	void Finalize();

	///
	/// User Method
	/// 

	/// <summary>
	/// 敵の発生
	/// </summary>
	/// <param name="pos">発生する座標</param>
	void SpawnEnemy(Vector3 pos);

#pragma region csvの読み込みと更新
	// 読み込み
	void LoadEnemyPopData();
	// 更新
	void UpdateEnemyPopCommands();
#pragma endregion

	/// <summary>
	/// 弾のアドレスを登録
	/// </summary>
	/// <param name="enemyBullet">弾</param>
	void AddEnemyBullet(EnemyBullet* enemyBullet) {
		// リストに登録する
		enemyBullets_.push_back(enemyBullet);
	}

	///
	/// Setter
	/// 

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

private:// プライベートなメンバ変数
	// カメラのアドレス
	Camera* camera_;
	// 衝突マネージャのアドレス
	CollisionManager* collisionManager_;
	// 自機のアドレス
	Player* player_;

	// 敵
	std::list<Enemy*> enemy_;
	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;

	// 使用するモデル
	std::vector<Model*> models_;

	// 敵の発生時間や座標などのCSVファイル
	std::stringstream enemyPopCommands_;
	// 敵が発生待機中か
	bool isWait_ = false;
	// 敵が発生するまでの時間
	int32_t waitTime_ = 0;
};