#pragma once
#include "MathStructs.h"
#include "Player.h"
#include "CollisionManager.h"

class EnemyManager;
class IEnemy {
public:
	IEnemy() = default;
	virtual ~IEnemy() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(Vector3 pos, Vector3 rotate, int id) = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	// 管理番号の取得
	int GetId() { return id_; }
	// 完了ならtrueを返す
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 自機のアドレスを設定
	/// </summary>
	/// <param name="player">自機のアドレス</param>
	void SetPlayer(Player* player) { player_ = player; }
	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// 衝突マネージャのアドレスを設定
	void SetCollisionManager(CollisionManager* collisionManager) { collisionManager_ = collisionManager; }
	// エネミーマネージャのアドレスを設定
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
	// 死亡フラグを設定
	void SetIsDead(bool isDead) { isDead_ = isDead; }

protected:	
	// 全てのモデル(見た目のデータ)
	std::vector<Model*> models_;
	// 敵の3DObject
	std::unique_ptr<Object3D> object3d_;

	Player* player_;
	// カメラのアドレス
	Camera* camera_;
	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;
	// エネミーマネージャ
	EnemyManager* enemyManager_ = nullptr;

	// 死亡フラグ
	bool isDead_ = false;
	float hp_ = 100;

	// 管理番号
	int id_;
};