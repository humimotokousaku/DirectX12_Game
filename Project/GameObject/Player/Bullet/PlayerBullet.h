#pragma once
#include "Collider.h"
#include "CollisionManager.h"
#include "Object3D.h"
#include "WorldTransform.h"
#include "GameTime.h"
#include "BasePlayerBulletState.h"

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

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider);

	/// <summary>
	/// stateの変更
	/// </summary>
	/// <param name="pState">state</param>
	void ChangeState(BasePlayerBulletState* pState);


#pragma region Getter
	/// <summary>
	/// 追尾対象のワールドトランスフォームの情報
	/// </summary>
	/// <returns></returns>
	WorldTransform* GetEnemyData() { return enemyData_; }

	/// <summary>
	/// 弾本体のワールドトランスフォームの取得
	/// </summary>
	/// <returns></returns>
	WorldTransform GetWorldTransform() { return object3d_->worldTransform; }

	/// <summary>
	/// 移動速度を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetVelocity() { return velocity_; }

	/// <summary>
	/// ワールド行列の平行移動成分を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition();
	/// <summary>
	/// オイラー角を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetRotation();

	/// <summary>
	/// 弾の速度を取得
	/// </summary>
	/// <returns></returns>
	const float GetBulletSpeed() { return kBulletSpeed; }

	// 死亡フラグの取得
	bool IsDead() const { return isDead_; }
#pragma endregion

#pragma region Setter
	/// <summary>
	/// カメラのアドレスを設定
	/// </summary>
	/// <param name="camera">カメラのアドレス</param>
	void SetCamera(Camera* camera) { camera_ = camera; }
	/// <summary>
	/// 自機のアドレスを設定
	/// </summary>
	/// <param name="player">自機のアドレス</param>
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// 移動速度を設定
	/// </summary>
	/// <param name="velocity">移動速度</param>
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }

	/// <summary>
	/// 死亡フラグの設定
	/// </summary>
	/// <param name="isDead">死んでいるか</param>
	void SetIsDead(bool isDead) { isDead_ = isDead; }
#pragma endregion

private:// 定数
	// 弾が最初にぶ方向
	const Vector3 kFirstVelocity = { 0.0f,0.0f,1.0f };
	// 弾の速度
	const float kBulletSpeed = 15.0f;

	// 弾の大きさ
	const Vector3 kBulletSize = { 3.0f,3.0f,3.0f };
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

	// 弾の状態
	BasePlayerBulletState* state_;

	// 速度
	Vector3 velocity_;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 1;
	// 死亡タイマー
	float deathTimer_ = kLifeTime;
	// 死亡フラグ
	bool isDead_ = false;
};
