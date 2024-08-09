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

	// 敵モデルを追加
	void AddModel(Model* model) {
		models_.push_back(model);
	}

	// 管理番号の取得
	int GetId() { return id_; }
	// 移動速度を取得
	Vector3 GetMoveSpeed() { return moveSpeed_; }
	// 完了ならtrueを返す
	bool IsDead() const { return isDead_; }

	Vector3 GetWorldPosition() {
		// ワールド座標を入れる変数
		Vector3 worldPos{};
		// ワールド行列の平行移動成分を取得
		worldPos.x = object3d_->worldTransform.matWorld_.m[3][0];
		worldPos.y = object3d_->worldTransform.matWorld_.m[3][1];
		worldPos.z = object3d_->worldTransform.matWorld_.m[3][2];

		return worldPos;
	}

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
	// 移動速度を設定
	void SetMoveSpeed(Vector3 speed) { moveSpeed_ = speed; }
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
	// 被弾時時のパーティクル
	//std::vector<Particles*> hitParticles_;

	// 死亡フラグ
	bool isDead_ = false;
	float hp_ = 100;

	// 管理番号
	int id_;

	// 敵のタイプ
	std::string type_;
	// 移動速度
	Vector3 moveSpeed_;
};