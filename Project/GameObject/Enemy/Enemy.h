#pragma once
#include "Collider.h"
#include "Camera.h"
#include "Object3D.h"
#include "IEnemyState.h"
#include "EnemyBullet.h"
#include "CollisionManager.h"
#include <vector>

class Player;
//class GameScene;
class EnemyManager;
class Enemy : public Collider
{
public:
	Enemy();
	~Enemy();

	/// <summary>
	/// 初期化(設定すべき項目を全て終わらせてから行う)
	/// <summary>
	void Initialize(Vector3 pos, Vector3 rotate);

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// 描画
	/// <summary>
	void Draw();

	/// <summary>
	/// stateの変更
	/// </summary>
	/// <param name="pState">state</param>
	void ChangeState(IEnemyState* pState);

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="velocity">速度</param>
	void Move(const Vector3 velocity);

	/// <summary>
	/// 発射処理
	/// </summary>
	void Fire();

	///
	/// Getter
	/// 

	// 座標を取得
	Vector3 GetEnemyPos() { return object3d_->worldTransform.translate; }

	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }

	// 完了ならtrueを返す
	bool IsDead() const { return isDead_; }

	/// 
	/// Setter
	/// 

	// 敵モデルを追加
	void AddModel(Model* model) {
		models_.push_back(model);
	}

	// 自機のアドレスを設定
	void SetPlayer(Player* player) { player_ = player; }
	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// ゲームシーンのアドレスを設定
	//void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	// 衝突マネージャのアドレスを設定
	void SetCollisionManager(CollisionManager* collisionManager) { collisionManager_ = collisionManager; }
	// エネミーマネージャのアドレスを設定
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }

	// 死亡フラグを設定
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	// 座標を設定
	void SetPosition(Vector3 pos) { object3d_->worldTransform.translate = pos; }

	///
	/// 純粋仮想関数
	/// 

	// ワールド座標
	Vector3 GetWorldPosition() override;
	// 角度
	Vector3 GetRotation() override;
	// 衝突判定
	void OnCollision(Collider* collider)override;

private:
	// 全てのモデル(見た目のデータ)
	std::vector<Model*> models_;
	// 敵の3DObject
	std::unique_ptr<Object3D> object3d_;

	// 敵のテクスチャ
	uint32_t enemyTexture_;

	// 弾
	std::list<EnemyBullet*> bullets_;
	// 自キャラのアドレス
	Player* player_ = nullptr;
	// カメラのアドレス
	Camera* camera_;
	// ゲームシーンのアドレス
	//GameScene* gameScene_ = nullptr;
	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;
	// エネミーマネージャ
	EnemyManager* enemyManager_ = nullptr;

	// 状態遷移
	IEnemyState* state_;

	// 死亡フラグ
	bool isDead_ = false;
};

