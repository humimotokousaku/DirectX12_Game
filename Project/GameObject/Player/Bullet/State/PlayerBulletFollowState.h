#pragma once
#include "BasePlayerBulletState.h"

class PlayerBulletFollowState : public BasePlayerBulletState {
public:
	PlayerBulletFollowState(PlayerBullet* playerBullet, Player* player);
	~PlayerBulletFollowState() = default;
	void Initialize() override;
	void Update() override;

private:
	// 追尾精度(0~1)
	float followRate_;

};