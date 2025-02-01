#pragma once
#include "BasePlayerBulletState.h"
#include "MathStructs.h"
#include "Animation.h"

class PlayerBulletNormalState : public BasePlayerBulletState {
public:
	PlayerBulletNormalState(PlayerBullet* playerBullet, Player* player, const Vector3& shotAngle);
	~PlayerBulletNormalState() = default;

	void Initialize() override;

	void Update() override;

public:// 定数
	// 弾の移動速度
	const Vector3 kBulletVelocity = { 0.0f,1.0f,0.0f };

	// 次の状態に遷移するまでの時間
	const float kChangeStateInterval = 20;

	// 弾の速さ
	const float kBulletSpeed = 1.0f;

private:
	// 速度変化
	Animation velocityAnim_;

	// 現在の座標
	Vector3 currentPos_;

	// 速度
	Vector3 velocity_;

	float currentFrame_;
};

