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
#include "EvasionSystem.h"
#include "GlobalVariables.h"
#include <map>
#include <tuple>

class GameSystem;
/// <summary>
/// 自機
/// </summary>
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
	
	/// <summary>
	/// タイトル演出時の挙動
	/// </summary>
	void TitleEffect(bool& isEnd);
	/// <summary>
	/// クリア演出時の挙動
	/// </summary>
	void ClearEffect(bool& isEnd);
	/// <summary>
	/// ゲームオーバー時のアニメーション
	/// </summary>
	void DeadEffect(bool& isEnd);

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
	/// 自機の移動制限
	/// </summary>
	void MoveLimit();

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

	// 衝突判定
	void OnCollision(Collider* collider);

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
	void DecrementHP(Collider* collider);

	/// <summary>
	/// 弾ゲージの更新処理
	/// </summary>
	void BulletGaugeUpdate();

	/// <summary>
	/// 全てのパーティクル情報を読み込む
	/// </summary>
	void LoadParticlesData();

	/// <summary>
	/// ImGuiの表示
	/// </summary>
	void ImGuiParameter();

public:// GetterとSetter
#pragma region Getter
	// 死亡フラグを取得
	const bool& GetIsDead() { return isDead_; }
	// ワールド座標
	Vector3 GetWorldPosition();
	// 角度
	const Vector3& GetRotation();

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }
	// 体のワールドトランスフォームを取得
	const WorldTransform* GetWorldTransform() { return &object3d_->worldTransform; }
	// 弾ゲージの情報を取得
	const GaugeData& GetBulletGauge() { return bulletGauge_; }
	// 回避時の情報を取得
	const EvasionData& GetEvasionData() { return evasionSystem_->GetEvasionData(); }
	// 死亡演出時の情報を取得
	const DeadAnimationData& GetDeadAnimationData() { return deadAnim_; }
	// カメラの移動幅
	const Vector3& GetCameraOffset() { return cameraOffset_; }
	// カメラの回転幅
	const Vector3& GetCameraRotateOffset() { return cameraRotateOffset_; }
	// 移動ベクトルのアドレスを取得
	const Vector3& GetMoveVel() { return moveVel_; }
	// 加速モードかを取得
	const bool& GetIsBoost() { return boost_.isActive; }
	// 回避中かを取得
	const bool& GetIsEvasion() { return evasionSystem_->GetIsEvasion(); }
	// 被弾演出中かを取得
	const bool& GetIsHit() { return hitSystem_->GetIsHit(); }

	/// <summary>
	/// 初めてのジャスト回避を行っているか最中か
	/// </summary>
	/// <returns></returns>
	bool GetFirstJustEvasion() {
		return evasionSystem_->GetFirstJustEvasion();
	}
	int GetFirstJustEvasionState() {
		return evasionSystem_->GetFirstJustEvasionState();
	}
#pragma endregion

#pragma region Setter
	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// ゲームシステムクラスのアドレスを設定
	void SetGameScene(GameSystem* gameSystem) { gameSystem_ = gameSystem; }

	/// <summary>
	/// 親となるワールドトランスフォームを設定
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// ローカル座標を設定
	/// </summary>
	/// <param name="pos"></param>
	void SetLocalPosition(const Vector3& pos) { object3d_->worldTransform.translate = pos; }

	/// <summary>
	/// カメラの移動幅の設定
	/// </summary>
	/// <param name="cameraOffset">カメラのオフセット</param>
	void SetCameraOffset(const Vector3& cameraOffset) { cameraOffset_ = cameraOffset; }

	/// <summary>
	/// 自機の当たり判定をとるかを設定
	/// </summary>
	/// <param name="isActive"></param>
	void SetIsCollion(const bool& isActive) { object3d_->collider->SetIsActive(isActive); }
#pragma endregion
	// 使用するモデルを追加
	void AddModel(std::string modelName, Model* model) { models_.insert(std::make_pair(modelName, std::move(model))); }

	/// <summary>
	/// 弾ゲージの増加量
	/// </summary>
	void IncrementBulletGauge(float value) { bulletGauge_.value += value; }

	/// <summary>
	/// 自機の角度を0にする
	/// </summary>
	void ResetAngle() {
		object3d_->worldTransform.rotate = { 0,0,0 };
		object3d_->worldTransform.UpdateMatrix();
	}

private:// プライベートなメンバ変数
	// ゲームタイマー
	GameTimer* gameTimer_;
	// キーボード入力
	Input* input_ = nullptr;
	// 音
	Audio* audio_;
	// ゲームパッド
	XINPUT_STATE joyState_;
	// jsonファイルに保存したデータ
	GlobalVariables* globalVariables_;

private:// プライベートなメンバ変数
#pragma region 他のクラスのアドレス
	// カメラのアドレス
	Camera* camera_;
	// ゲームシーンのアドレス
	GameSystem* gameSystem_;
	// 衝突マネージャのアドレス
	CollisionManager* collisionManager_;
#pragma endregion

	// 自機
	std::unique_ptr<Object3D> object3d_;

	// 自機の軌道パーティクル
	std::array<std::unique_ptr<Particles>, kMaxOrbitNum> particles_;
	// 死亡時のパーティクル
	std::unique_ptr<Particles> deadParticle_;
	// 弾
	std::list<PlayerBullet*> bullets_;

	// 使用するモデル
	std::map<std::string, Model*> models_;

	// 被弾時の演出
	std::unique_ptr<HitSystem> hitSystem_;
	// 回避システム
	std::unique_ptr<EvasionSystem> evasionSystem_;

	// 加速時の情報
	BoostData boost_{};

#pragma region アニメーション
	// ブースト時のアニメーション
	Animation boostRotAnim_;

	// やられアニメーション
	DeadAnimationData deadAnim_;

	// タイトルシーンからゲームシーンになるまでのアニメーション
	Animation titleAnim_;
	// クリア時のアニメーション
	Animation clearAnim_;
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
	ParticleTextures deadParticleTextures_;
#pragma endregion

	// 自機の移動速度
	Vector3 moveVel_;
	//自機の回転速度
	Vector3 rotateVel_;

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

	// 無敵中か
	bool isInvinsible_ = false;
	// 死亡フラグ
	bool isDead_ = true;
};