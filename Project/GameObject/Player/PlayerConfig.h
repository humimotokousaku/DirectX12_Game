#pragma once
#include "MathStructs.h"
#include "Sprite.h"
#include "Animation.h"
#include <numbers>

#pragma region 定数
// 自機の移動速度の減衰率
const Vector3 kMoveSpeedDecayRate = { 3.0f, 3.0f, 1.0f };
// 自機の回転速度の減衰率
const Vector3 kRotateSpeedDecayRate = { 0.3f,0.3f,1.4f };

// 自機の最大移動速度
const float kMaxSpeed = 1.0f;
// 自機の最大回転速度
const Vector3 kMaxRotSpeed = { 1.0f, 1.0f, 1.0f };
// 自機の最大回避速度
const Vector2 kMaxEvasionMoveSpeed = { 4.0f, 4.0f };
// 回避時の回転数[2π * n回]
const Vector3 kMaxEvasionRotNum = { 0.0f, 0.0f ,(2.0f * (float)std::numbers::pi) * 2.0f };

// 移動限界座標
const Vector3 kMoveLimit = { 35.0f, 17.0f, 5.0f };

#pragma region カメラ演出
// 回転幅
const Vector3 kMaxCameraRotDirection = { 0.1f,0.2f,0.1f };
#pragma endregion

// カメラから照準オブジェクトの距離
const float kDistanceObject = 50.0f;

#pragma region 自機のゲージ類
// HPスプライトの最大サイズ
const Vector2 kMaxHPSize = { 300.0f, 32.0f };
// HPの最大値
const float kMaxHp = 100;

// 弾ゲージの最大サイズ
const Vector2 kMaxBulletGaugeSize = { 250.0f, 16.0f };
// 弾ゲージの最大値
const float kMaxBulletGauge = 100;
// 弾ゲージの増加量
const float kIncrementBulletGauge = 0.1f;
// 弾ゲージの減少量
const float kDecrementBulletGauge = -0.2f;
// 弾ゲージの倍率
const float kMagnificationBulletGauge = 2.0f;
#pragma endregion

// 弾の発射間隔[frame]
const float kBulletInterval = 5;

// 無敵時間[frame]
const float kMaxInvinsibleFrame = 60;

// 残像を表示する時間[frame]
const float kMaxEvasionFrame = 15;
// ジャスト回避の全体時間[frame]
const float kJustEvasionAllFrame = 33;
// ジャスト回避猶予時間[frame]
const float kMaxJustEvasionFrame = 8;

// 死亡演出の時間
const float kMaxDeadAnimationFrame = 240;
// 死亡演出時のヒットストップ時間
const float kMaxDeadHitStopFrame = 33;
#pragma endregion

#pragma region 構造体
// 加速時の情報
struct BoostData {
	// ブースト中の自機の回転
	float rotVelZ;
	// ブースト時の移動速度
	float moveSpeed;
	// 加速モード
	bool isActive = false;

	// 全ての情報初期化
	void Reset() {
		// ブースト中の自機の回転
		rotVelZ = 0.0f;
		// ブースト時の移動速度
		moveSpeed = 0.0f;
		// 加速モード
		isActive = false;
	}
};
// 回避時の情報
struct EvasionData {
	// 回避した瞬間の回転ベクトル
	Vector3 rotVel;
	// 座標の補正
	Vector3 offset;
	// 回避速度
	Vector2 moveSpeed;
	// 回避方向
	Vector2 direction;
	// 回避の経過時間[frame]
	float curretFrame = kMaxEvasionFrame;
	// ジャスト回避の演出時間[frame]
	float justCurrentFrame = kJustEvasionAllFrame;
	// ジャスト回避の猶予時間[frame]
	float justFrame = kMaxJustEvasionFrame;
	// 回避モード
	bool isActive = false;
	// ジャスト回避中か
	bool isJust = false;
	// 残像のα値
	std::vector<float> alphas;

	// 全ての情報初期化
	void Reset() {
		// 回避した瞬間の回転ベクトル
		rotVel = { 0,0,0 };
		// 座標の補正
		offset = { 0,0,0 };
		// 回避速度
		moveSpeed = { 0,0 };
		// 回避方向
		direction = { 0,0 };
		// 回避の経過時間[frame]
		curretFrame = kMaxEvasionFrame;
		// ジャスト回避の演出時間[frame]
		justCurrentFrame = kJustEvasionAllFrame;
		// ジャスト回避の猶予時間[frame]
		justFrame = kMaxJustEvasionFrame;
		// 回避モード
		isActive = false;
		// ジャスト回避中か
		isJust = false;
		// 残像のα値
		for (float& alpha : alphas) {
			alpha = 0.0f;
		}
	}
	// ジャスト回避の情報のみ初期化
	void JustDataReset() {
		// ジャスト回避の演出時間[frame]
		justCurrentFrame = kJustEvasionAllFrame;
		// ジャスト回避の猶予時間[frame]
		justFrame = kMaxJustEvasionFrame;
		// ジャスト回避中か
		isJust = false;
	}
};
// ゲージの情報
struct GaugeData {
	std::unique_ptr<Sprite> sprite;			// ゲージの描画
	Vector2 size;			// ゲージの大きさ
	float incrementValue;	// ゲージの上昇量
	float magnification;	// ゲージの上昇倍率
	float value;			// 値
	bool isMax;				// ゲージが最大までたまっているか
};
// ゲームオーバー時の演出
struct DeadAnimationData {
	Animation animation;// 死亡アニメーション
	Vector3 velocity;	// 速度
	Vector3 rotate;		// 回転
	float currentFrame = kMaxDeadAnimationFrame; // 経過時間
	bool isEnd;			// アニメーションが終了している
};
#pragma endregion