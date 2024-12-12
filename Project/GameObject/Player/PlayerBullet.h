#pragma once
#include "Collider.h"
#include "CollisionManager.h"
#include "Object3D.h"
#include "WorldTransform.h"
#include "GameTime.h"

class IEnemy;
class Player;
/// <summary>
/// 自機の弾
/// </summary>
class PlayerBullet{
public: // メンバ関数

	PlayerBullet() = default;
	~PlayerBullet();

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
	void Draw();

	///
	/// Getter
	/// 
	
	// 死亡フラグの取得
	bool IsDead() const { return isDead_; }

	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition();
	// 角度
	Vector3 GetRotation();

	///
	/// Setter
	/// 

	// 死亡フラグの設定
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// 自機のアドレス
	void SetPlayer(Player* player) { player_ = player; }

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider);

private:// 定数
	// 弾が最初にぶ方向
	const Vector3 kFirstVelocity = { 0.0f,0.0f,1.0f };
	// 弾の速度
	const float kBulletSpeed = 15.0f;

	// 弾の大きさ
	const Vector3 kBulletSize = { 2.0f,2.0f,2.0f };
	// 弾のダメージ
	const float kBulletDamage = 1000.0f;

private: // メンバ変数
	// モデル
	std::unique_ptr<Object3D> object3d_ = nullptr;

	// 自機のアドレス
	Player* player_;
	// カメラのアドレス
	Camera* camera_;
	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;
	// 追尾対象の敵情報
	WorldTransform* enemyData_;

	// 速度
	Vector3 velocity_;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 3;
	// 死亡タイマー
	float deathTimer_ = kLifeTime;
	// 死亡フラグ
	bool isDead_ = false;
};
