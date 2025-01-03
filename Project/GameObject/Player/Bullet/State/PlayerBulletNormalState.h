#pragma once
#include "BasePlayerBulletState.h"
#include "MathStructs.h"

class PlayerBulletNormalState : public BasePlayerBulletState {
public:
	PlayerBulletNormalState(PlayerBullet* playerBullet, Player* player);
	~PlayerBulletNormalState() = default;

	void Initialize() override;

	void Update() override;

public:// 定数
	// 弾の移動速度
	const Vector3 kBulletVelocity = { 0.0f,0.0f,1.0f };

	// 次の状態に遷移するまでの時間
	const float kChangeStateInterval = 15;

private:
	float currentFrame_;
};

