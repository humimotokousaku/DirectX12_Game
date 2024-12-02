#pragma once
#include "Animation.h"
#include "Audio.h"
#include "Camera.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "Input.h"
#include "Object3D.h"
#include "Particles.h"
#include "PlayerBullet.h"
#include "PlayerConfig.h"
#include "Sprite.h"
#include "GameTime.h"
#include "Shake.h"
#include "HitSystem.h"
#include "JustEvasionSystem.h"
#include <map>
#include <tuple>

class GameSystem;
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
private:// プライベートなメンバ関数
	/// <summary>
	/// キーボードとコントローラーの入力処理
	/// </summary>
	void InputUpdate(Vector3& move, Vector3& rotate);
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

	/// <summary>
	/// 加速中の更新処理
	/// </summary>
	/// <param name="moveZ">Z方向の移動ベクトル</param>
	void BoostUpdate(float moveZ);

	/// <summary>
	/// 回避の更新処理
	/// </summary>
	/// <param name="rotateY">自機のY軸回転角</param>
	/// <param name="rotateX">自機のX軸回転角</param>
	void EvasionUpdate(float rotateY, float rotateX);

	/// <summary>
	/// ジャスト回避
	/// </summary>
	void JustEvasion();
	/// <summary>
	/// ジャスト回避成功フレーム中かを検出
	/// </summary>
	/// <returns></returns>
	bool IsJustEvasionFrame();
	/// <summary>
	/// 初めてジャスト回避を行うときの処理
	/// </summary>
	void FirstJustEvasion();

	// 衝突判定
	void OnCollision(Collider* collider);
	// ジャスト回避の判定
	void JustOnCollision(Collider* collider);

	// ジャスト回避時のパーティクルの更新処理
	void JustParticleUpdate(Particle& particle);

	/// <summary>
	/// 自機の無敵状態の処理
	/// </summary>
	void InvinsibleUpdate();

	/// <summary>
	/// HPゲージの更新処理
	/// </summary>
	void HPUpdate();
	/// <summary>
	/// HPの減少処理
	/// </summary>
	void DecrementHP();

	/// <summary>
	/// 弾ゲージの更新処理
	/// </summary>
	void BulletGaugeUpdate();

	/// <summary>
	/// ゲームオーバー時のアニメーション
	/// </summary>
	void DeadAnimation();

	/// <summary>
	/// ImGuiの表示
	/// </summary>
	void ImGuiParameter();

public:// GetterとSetter
#pragma region Getter
	// 死亡フラグを取得
	const bool& GetIsDead() { return isDead_; }
	// ワールド座標
	const Vector3& GetWorldPosition();
	// 角度
	const Vector3& GetRotation();

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }
	// 体のワールドトランスフォームを取得
	const WorldTransform* GetWorldTransform() { return &object3d_->worldTransform; }
	// 弾ゲージの情報を取得
	const GaugeData& GetBulletGauge() { return bulletGauge_; }
	// 回避時の情報を取得
	const EvasionData& GetEvasionData() { return evasion_; }
	// 死亡演出時の情報を取得
	const DeadAnimationData& GetDeadAnimationData() { return deadAnim_; }
	// カメラの移動幅
	Vector3* GetCameraOffset_P() { return &cameraOffset_; }
	// カメラの回転幅
	Vector3* GetCameraRotateOffset_P() { return &cameraRotateOffset_; }
	// 移動ベクトルのアドレスを取得
	Vector3* GetMoveVel_P() { return &moveVel_; }
	// 加速モードかを取得
	bool* GetIsBoost_P() { return &boost_.isActive; }
	bool GetIsBoost() { return boost_.isActive; }
	// 回避中かを取得
	const bool& GetIsEvasion() { return evasion_.isActive; }
	// 被弾演出中かを取得
	const bool& GetIsHit() { return hitSystem_->GetIsHit(); }

	/// <summary>
	/// 初めてのジャスト回避を行っているか最中か
	/// </summary>
	/// <returns></returns>
	const bool& GetFirstJustEvasion() {
		if (isFirstJust_ == kNone) { return false; }
		else if (isFirstJust_ == kFirstJust) { return true; }
		else if (isFirstJust_ == kEnd) { return false; }
	}

#pragma endregion

#pragma region Setter
	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// ゲームシステムクラスのアドレスを設定
	void SetGameScene(GameSystem* gameSystem) { gameSystem_ = gameSystem; }
	// ロックオンフラグのアドレス取得
	void SetIsLockOn(bool* isLockOn) { isLockOn_ = isLockOn; }
	/// <summary>
	/// 親となるワールドトランスフォームを設定
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// ローカル座標を設定
	/// </summary>
	/// <param name="pos"></param>
	void SetLocalPosition(Vector3 pos) { object3d_->worldTransform.translate = pos; }
#pragma endregion
	// 使用するモデルを追加
	void AddModel(Model* model) { models_.push_back(model); }

	/// <summary>
	/// 弾ゲージの増加量
	/// </summary>
	void IncrementBulletGauge(float value) { bulletGauge_.value += value; }

private:// プライベートなメンバ変数
	// ゲームタイマー
	GameTimer* gameTimer_;
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
	// 自機の残像
	std::array<std::unique_ptr<Object3D>, 20> afterImageObject3d_;

	// ジャスト回避時のUI
	std::unique_ptr<Sprite> justSprite_;
	// 初めてジャスト回避するときのUI
	std::unique_ptr<Sprite> firstJustSprite_;

	// 自機の軌道パーティクル
	std::array<std::unique_ptr<Particles>, 2> particles_;
	// 死亡時のパーティクル
	std::unique_ptr<Particles> deadParticle_;
	// ジャスト回避時のパーティクル
	std::unique_ptr<Particles> justEvasionParticle_;
	// 弾
	std::list<PlayerBullet*> bullets_;

	// ジャスト回避の判定
	std::unique_ptr<Collider> justEvasionCollider_;

	// 被弾時の演出
	std::unique_ptr<HitSystem> hitSystem_;

	std::unique_ptr<JustEvasionSystem> justEvasionSystem_;

#pragma region アニメーション
	// ブースト時のアニメーション
	Animation boostRotAnim_;

	// 回避時の移動速度のアニメーション(イージングとして使用)
	Animation evasionSpeedAnim_;
	// 回避時の回転速度のアニメーション(イージングとして使用)
	Animation evasionRotSpeedAnim_;
	// 残像のα値のアニメーション
	std::vector<Animation> evasionAlphaAnims_;

	// やられアニメーション
	DeadAnimationData deadAnim_;
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
	// 死亡パーティクルのテクスチャ
	uint32_t deadParticleTexture = 0u;
#pragma endregion

#pragma region 他のクラスのアドレス
	// カメラのアドレス
	Camera* camera_;
	// ゲームシーンのアドレス
	GameSystem* gameSystem_;
	// 衝突マネージャのアドレス
	CollisionManager* collisionManager_;
#pragma endregion

	// 加速時の情報
	BoostData boost_{};

	// 回避時の情報
	EvasionData evasion_{};

	// 自機の移動速度
	Vector3 moveVel_;
	//自機の回転速度
	Vector3 rotateVel_;
	// レティクルの移動速度
	Vector3 reticleMoveVel_;

	// 被弾時の自機の角度
	Vector3 hitRotate_;

	// カメラの移動
	Vector3 cameraOffset_;
	// カメラの回転
	Vector3 cameraRotateOffset_;

	// HPゲージ
	GaugeData hp_;
	// 弾ゲージ
	GaugeData bulletGauge_;
	// 弾ゲージのy軸サイズ(アニメーション用)
	float bulletSizeY_;

	// 無敵時間
	float invinsibleFrame_;
	// ロックオン時のレティクルのサイズ変更に使用するframe
	int currentFrame_;


	float justFrame_;
	//float justAssistFrame;
	Vector3 justSize_ = { 1.0f,1.0f,37.0f };
	// 初めてジャスト回避を行うか
	//int isFirstJust_ = kNone;
	int isFirstJust_ = kEnd;

	// ロックオン
	bool* isLockOn_;
	// 無敵中か
	bool isInvinsible_ = false;
	// 死亡フラグ
	bool isDead_ = true;

};