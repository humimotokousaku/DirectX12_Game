#pragma once
#include "MathStructs.h"
#include <numbers>

/// 構造体
// 加速時の情報
struct BoostData {
	// ブースト中の自機の回転
	float rotVelZ;
	// ブースト時の移動速度
	float moveSpeed;
	// 加速モード
	bool isActive = false;
};
// 回避時の情報
struct EvasionData {
	// 回避した瞬間の移動ベクトル
	Vector2 moveVel;
	// 回避した瞬間の回転ベクトル
	Vector3 rotVel;
	// 回避速度
	Vector2 moveSpeed;
	// 回避時間
	int frame;
	// 回避モード
	bool isActive = false;
	// 残像のα値
	std::vector<float> alphas;
};

/// 定数
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

// HPスプライトの最大サイズ
const Vector2 kMaxHPSize = { 300.0f, 32.0f };
// HPの最大値
const float kMaxHp = 100;

// 弾の発射間隔[frame]
const int kBulletInterval = 5;
// 無敵時間[frame]
const int kMaxInvinsibleFrame = 60;
// 残像を表示する時間[frame]
const int kMaxEvasionFrame = 15;