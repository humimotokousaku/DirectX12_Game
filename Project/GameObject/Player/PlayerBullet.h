#pragma once
#include "Object3D.h"
#include "WorldTransform.h"
#include "Collider.h"
#include "CollisionManager.h"

/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet : public Collider{
public: // メンバ関数

	PlayerBullet() = default;
	~PlayerBullet();

	/// <summary>
	/// 初期化(設定すべき項目を全て終わらせてから行う)
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="pos">初期座標</param>
	void Initialize(Model* model, const Vector3& pos, const Vector3& velocity);

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
	/// Getter
	/// 
	
	// 死亡フラグの取得
	bool IsDead() const { return isDead_; }

	///
	/// Setter
	/// 

	// 死亡フラグの設定
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// 衝突マネージャのアドレスを設定
	void SetCollisionManager(CollisionManager* collisionManager) { collisionManager_ = collisionManager; }

	///
	/// 純粋仮想関数
	/// 

	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition() override;
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;
	// 角度
	Vector3 GetRotation() override;


private: // メンバ変数
	// モデル
	std::unique_ptr<Object3D> object3d_ = nullptr;

	// カメラのアドレス
	Camera* camera_;
	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 5;
	// 死亡タイマー
	int32_t deathTimer_ = kLifeTime;
	// 死亡フラグ
	bool isDead_ = false;

	// 速度
	Vector3 velocity_;
};
