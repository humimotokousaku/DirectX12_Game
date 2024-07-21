#pragma once
#include "Object3D.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "Camera.h"
#include "Sprite.h"
#include "Object3D.h"
#include "Particles.h"
#include "Input.h"
#include "PlayerBullet.h"

class AimAssist;
class GameScene;
class Player : public Collider {
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

public:
#pragma region Getter
	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }
	// 3Dレティクルの座標
	Vector3 GetWorld3DReticlePosition();
	// カメラの移動幅
	Vector3* GetCameraOffset() { return &cameraOffset_; }
	// カメラの回転幅
	Vector3* GetCameraRotateOffset() { return &cameraRotateOffset_; }
	/// <summary>
	/// 体のワールドトランスフォームを取得
	/// </summary>
	/// <returns></returns>
	const WorldTransform* GetWorldTransform() { return &object3d_->worldTransform; }
#pragma endregion

#pragma region Setter
	// モデルを追加
	void AddModel(Model* model) { models_.push_back(model); }

	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// ゲームシーンのアドレスを設定
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	// 衝突マネージャのアドレスを設定
	void SetCollisionManager(CollisionManager* collisionManager) { collisionManager_ = collisionManager; }

	void SetPos(Vector3 pos) { object3d_->worldTransform.translate = pos; }
	void SetReticle3DPos(Vector3 pos) { object3dReticle_->worldTransform.translate = pos; }

	/// <summary>
	/// 親となるワールドトランスフォームを設定
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);
#pragma endregion

	///
	/// 純粋仮想関数
	/// 

	// ワールド座標
	Vector3 GetWorldPosition() override;
	// 角度
	Vector3 GetRotation() override;
	// 衝突判定
	void OnCollision(Collider* collider)override;

private:// プライベートなメンバ関数
	/// <summary>
	/// HPの減少処理
	/// </summary>
	void DecrementHP();

	/// <summary>
	/// レティクルの配置
	/// </summary>
	void Deploy3DReticle();
	/// <summary>
	/// 2Dレティクルの配置
	/// </summary>
	void Deploy2DReticle();

#pragma region 入力処理
	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();
	/// <summary>
	/// 自機の回転処理
	/// </summary>
	//void Rotate();
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
	const Vector3 kRotateSpeedAttenuationRate = { 0.6f,0.6f,0.6f };

	// 自機の最大移動速度
	const float kMaxSpeed = 0.1f;
	// 自機の最大回転速度
	const float kMaxRotSpeed = 0.9f;

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

	// 無敵時間
	const int kMaxInvinsibleFrame = 60;

private:// プライベートなメンバ変数
	// キーボード入力
	Input* input_ = nullptr;
	// ゲームパッド
	XINPUT_STATE joyState_;

	// 自機
	std::unique_ptr<Object3D> object3d_;
	std::vector<Model*> models_;
	// 2Dレティクル用のスプライト
	Sprite* sprite2DReticle_ = nullptr;
	// HP用のスプライト
	Sprite* hpSprite_ = nullptr;
	// 自機の軌道パーティクル
	std::unique_ptr<Particles> particle_;

	// レティクルハンドル
	uint32_t reticleTexture_ = 0u;
	// 自機のスプライト
	uint32_t playerTexture_ = 0u;
	// パーティクルテクスチャ
	uint32_t defaultTexture = 0u;

	// 弾
	std::list<PlayerBullet*> bullets_;

	// カメラのアドレス
	Camera* camera_;
	// ゲームシーンのアドレス
	GameScene* gameScene_;
	// エイムアシストのアドレス
	AimAssist* aimAssist_;
	// 衝突マネージャのアドレス
	CollisionManager* collisionManager_;

	bool isDead_ = true;

	float gameSpeed_;

	// 3Dレティクルの座標
	std::unique_ptr<Object3D> object3dReticle_;

	// 自機の移動速度
	Vector3 moveVel_;
	// レティクルの移動速度
	Vector3 reticleMoveVel_;
	//自機の回転速度
	Vector3 rotateVel_;
	Vector3 kRotateSpeedRate = { 0.5f,0.5f,0.5f };

#pragma region カメラ演出
	// カメラの移動
	Vector3 cameraOffset_;
	// カメラの回転
	Vector3 cameraRotateOffset_;
#pragma endregion

	// 横方向
	bool isVertical_;
	bool isHorizontal_;

	// 弾の発射間隔
	int bulletInterval_;

	// HPテクスチャのサイズ
	Vector2 hpSize_;

	// 無敵中か
	bool isInvinsible_;
	// 無敵時間
	int invinsibleFrame_;
};