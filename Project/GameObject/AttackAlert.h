#pragma once
#include "Sprite.h"
#include "Animation.h"
#include "PostEffectManager.h"
#include "Player.h"

/// <summary>
/// 強い攻撃を警告するクラス
/// </summary>
class AttackAlert {
public:
	AttackAlert() = default;
	~AttackAlert() = default;

	// シングルトン
	static AttackAlert* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	///
	/// User Method
	/// 

	/// <summary>
	/// ロックオン対象にレティクルを合わせる
	/// </summary>
	void LockOnReticle();

	// アラート開始
	void Start() {
		isStart_ = true;
		isLockOn_ = true;
		// スプライトを描画する
		for (int i = 0; i < alertSprite_.size(); i++) {
			alertSprite_[i].isActive_ = true;
		}
		for (int i = 0; i < sprite2DReticle_.size(); i++) {
			sprite2DReticle_[i].isActive_ = true;
		}
		// 各アニメーション起動
		for (int i = 0; i < alertAnim_.size(); i++) {
			alertAnim_[i].SetIsStart(true);
		}
		reticleAnim_.SetIsStart(true);
	}
	// アラートを強制終了
	void End() {
		isStart_ = false;
		isLockOn_ = false;
		currentFrame_ = kFinishAlert;
		alertSprite_[0].uvTransform_.translate = { 0,0,0 };
		for (int i = 0; i < alertSprite_.size(); i++) {
			alertSprite_[i].isActive_ = false;
		}
		for (int i = 0; i < sprite2DReticle_.size(); i++) {
			sprite2DReticle_[i].isActive_ = false;
		}
		// 各アニメーションを終了
		for (int i = 0; i < alertAnim_.size(); i++) {
			alertAnim_[i].SetIsStart(false);
		}
		reticleAnim_.SetIsStart(false);
	}

#pragma region Setter
	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// 自機のアドレスを設定
	void SetPlayer(Player* player) { player_ = player; }
#pragma endregion

private:// エンジン機能
	PostEffectManager* postEffectManager_;

private:// 定数
	// アラート表示時間
	const int kFinishAlert = 240;
	// ロックオン時間
	const int kLockOnFrame = 180;

private:
	// カメラのアドレス
	Camera* camera_;
	// 自機のアドレス
	Player* player_;

	// アラート画像
	std::array<Sprite, 2> alertSprite_;
	// ロックオンレティクル用のスプライト
	std::array<Sprite, 2> sprite2DReticle_;

	// アラートのアニメーション
	std::array<Animation, 2> alertAnim_;
	// ロックオン時のアニメーション
	Animation reticleAnim_;

	// ロックオン対象の座標
	Vector3 targetPos_;

	// アラート起動
	bool isStart_;
	// 自機のロックオン起動
	bool isLockOn_ = false;

	// 経過時間
	int currentFrame_;
};