#pragma once
#include "Collider.h"
#include "CollisionManager.h"
#include "Model.h"
#include "Player.h"
#include "WorldTransform.h"
#include "GameTime.h"

class EnemyBullet {
public:// パブリックなメンバ関数
	///
	/// Default Method
	/// 

	EnemyBullet() = default;
	~EnemyBullet();

	/// <summary>
	/// 初期化(設定すべき項目を全て終わらせてから行う)
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="pos">初期座標</param>
	void Initialize(Model* model, const Vector3& pos, WorldTransform* enemyData);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw();

	///
	/// User Method
	/// 

	///
	/// Getter
	/// 

	// 死亡フラグを取得
	bool isDead() const { return isDead_; }

	///
	/// Setter
	/// 

	// 死亡フラグを設定
	void SetIsDead(bool isDead) { isDead_ = isDead; }
	
	// 自機のアドレスを設定
	void SetPlayer(Player* player) { player_ = player; }
	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider);
	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition();
	// 角度
	Vector3 GetRotation();

public:// 定数
	// 弾が最初にぶ方向
	const Vector3 kFirstVelocity = { 0.0f,0.0f,1.0f };
	// 弾の速度
	const float kBulletSpeed = 2.5f;

	// 追従の精度
	const float kFollowRate = 0.03f;

private: // プライベートなメンバ変数
	// モデル
	std::unique_ptr<Object3D> object3d_ = nullptr;

	// プレイヤーのアドレス
	Player* player_ = nullptr;
	// カメラのアドレス
	Camera* camera_;
	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;

	// 弾を撃つ敵のワールドトランスフォーム
	WorldTransform* enemyData_;

	// 速度
	Vector3 velocity_;

	Vector3 pos_;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 3;
	// 死亡タイマー
	int32_t deathTimer_ = kLifeTime;
	// 弾の有無
	bool isDead_ = false;
};