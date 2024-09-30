#pragma once
#include "Object3D.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "Camera.h"
#include "Sprite.h"
#include "Object3D.h"
#include "Particles.h"
#include "Input.h"
#include "Audio.h"
#include "PlayerBullet.h"
#include "Animation.h"

class AimAssist;
class GameScene;
class Player {
public:// パブリックなメンバ関数
	/// 
	/// Default Method
	/// 

	Player();
	~Player();

	/// <summary>
	/// 初期化(設定すべき項目を全て終わらせてから行う)
	/// <summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// <summary>
	void Update();
	/// <summary>
	/// 描画
	/// <summary>
	void Draw();
	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	///
	/// User Method
	/// 

#pragma region Getter
	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }
	// 3Dレティクルの座標
	Vector3 GetWorld3DReticlePosition(int index);
	// ロックオンされてない3Dレティクルの座標
	Vector3 GetDefault3DReticlePosition();
	// カメラの移動幅
	Vector3* GetCameraOffset() { return &cameraOffset_; }
	// カメラの回転幅
	Vector3* GetCameraRotateOffset() { return &cameraRotateOffset_; }
	// 加速モードかを取得
	bool* GetIsBoost() { return &isBoost_; }
	// 体のワールドトランスフォームを取得
	const WorldTransform* GetWorldTransform() { return &object3d_->worldTransform; }
	// 死亡フラグを取得
	bool GetIsDead() { return isDead_; }
	// ワールド座標
	Vector3 GetWorldPosition();
	// 角度
	Vector3 GetRotation();
#pragma endregion

#pragma region Setter
	// モデルを追加
	void AddModel(Model* model) { models_.push_back(model); }

	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// ゲームシーンのアドレスを設定
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	// ロックオンフラグのアドレス取得
	void SetIsLockOn(bool* isLockOn) { isLockOn_ = isLockOn; }
	/// <summary>
	/// ロックオン時のレティクルの補間量のアドレスを設定
	/// </summary>
	/// <param name="offset"></param>
	void SetLockOnReticleOffset(Vector3* offset) { lockOnReticleOffset_ = offset; }

	/// <summary>
	/// 親となるワールドトランスフォームを設定
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

	void SetPos(Vector3 pos) { object3d_->worldTransform.translate = pos; }
	void SetReticle3DPos(Vector3 pos) { object3dReticle_[0]->worldTransform.translate = pos; }

#pragma endregion

private:// プライベートなメンバ関数
	// 衝突判定
	void OnCollision(Collider* collider);

	/// <summary>
	/// HPの減少処理
	/// </summary>
	void DecrementHP();

	/// <summary>
	/// ゲームオーバー時のアニメーション
	/// </summary>
	void DeadAnimation();

	/// <summary>
	/// レティクルの配置
	/// </summary>
	void Deploy3DReticle();
	/// <summary>
	/// 2Dレティクルの配置
	/// </summary>
	void Deploy2DReticle();
	/// <summary>
	/// ロックオン時のレティクルの配置
	/// </summary>
	void DeployLockOnReticle();

#pragma region 入力処理
	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();
	/// <summary>
	/// レティクルの配置、移動の処理
	/// </summary>
	void Aim();
	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack();
#pragma endregion

public:// 定数
	// 自機の移動速度の減衰率
	const Vector3 kMoveSpeedAttenuationRate = { 4.0f, 4.0f, 4.0f };
	// 自機の回転速度の減衰率
	const Vector3 kRotateSpeedAttenuationRate = { 1.0f,1.0f,1.0f };

	// 自機の最大移動速度
	const float kMaxSpeed = 0.3f;
	// 自機の最大回転速度
	const Vector3 kMaxRotSpeed = { 0.5f , 0.7f, 1.0f };

	// 移動限界座標
	const Vector3 kMoveLimit = { 19.0f, 15.0f, 5.0f };

#pragma region カメラ演出
	// 回転幅
	const Vector3 kMaxCameraRotDirection = { 0.1f,0.1f,0.1f };

#pragma endregion

	// カメラから照準オブジェクトの距離
	const float kDistanceObject = 50.0f;

	// 弾の発射間隔[frame]
	const int kBulletInterval = 5;

	// HPスプライトの最大サイズ
	const Vector2 kMaxHPSize = { 300.0f, 32.0f };
	// HPの最大値
	const float kMaxHp = 100;

	// 無敵時間
	const int kMaxInvinsibleFrame = 60;

private:// プライベートなメンバ変数
	// キーボード入力
	Input* input_ = nullptr;
	// 音
	Audio* audio_;
	// ゲームパッド
	XINPUT_STATE joyState_;

	// 使用するモデル
	std::vector<Model*> models_;

private:// プライベートなメンバ変数
	// 自機
	std::unique_ptr<Object3D> object3d_;

	// 3Dレティクルの座標
	std::array<std::unique_ptr<Object3D>, 2> object3dReticle_;
	// ロックオンしていないときのレティクル座標
	WorldTransform default3dReticle_;

	// 2Dレティクル用のスプライト
	std::array<Sprite, 3> sprite2DReticle_;
	// HP用のスプライト
	Sprite hpSprite_;

	// 自機の軌道パーティクル
	std::array<std::unique_ptr<Particles>, 3> particle_;

	// 弾
	std::list<PlayerBullet*> bullets_;

#pragma region アニメーション
	// ロックオン時のアニメーション
	Animation reticleAnim_;
	// ブースト時のアニメーション
	Animation boostRotAnim_;
#pragma endregion

#pragma region テクスチャ
	// レティクルハンドル
	uint32_t reticleTexture_ = 0u;
	// ロックオン時のレティクル
	uint32_t lockOnReticleTexture_ = 0u;
	// 自機のスプライト
	uint32_t playerTexture_ = 0u;
	// パーティクルテクスチャ
	uint32_t defaultTexture = 0u;
#pragma endregion

#pragma region 音
	// 射撃SE
	uint32_t shotSE_;

	std::array<uint32_t, 3> dyingSE_;
#pragma endregion

#pragma region 他のクラスのアドレス
	// カメラのアドレス
	Camera* camera_;
	// ゲームシーンのアドレス
	GameScene* gameScene_;
	// エイムアシストのアドレス
	AimAssist* aimAssist_;
	// 衝突マネージャのアドレス
	CollisionManager* collisionManager_;
#pragma endregion

	// 自機の移動速度
	Vector3 moveVel_;
	// レティクルの移動速度
	Vector3 reticleMoveVel_;
	//自機の回転速度
	Vector3 rotateVel_;
	// ブースト中の自機の回転
	float boostRotVelZ_;

	// ロックオン時のレティクルの補間量
	Vector3* lockOnReticleOffset_;
	// カメラの移動
	Vector3 cameraOffset_;
	// カメラの回転
	Vector3 cameraRotateOffset_;

	// HPテクスチャのサイズ
	Vector2 hpSize_;
	// HP
	float hp_;

	// 無敵時間
	int invinsibleFrame_;
	// ロックオン時のレティクルのサイズ変更に使用するframe
	int currentFrame_;
	// 弾の発射間隔
	int bulletInterval_;

	// 無敵中か
	bool isInvinsible_;
	// 加速モード
	bool isBoost_;
	// ロックオン
	bool* isLockOn_;
	// 死亡フラグ
	bool isDead_ = true;
};