#include "FixedTurretWaitState.h"
#include "FixedTurretAttackState.h"
#include "FixedTurret.h"

FixedTurretWaitState::FixedTurretWaitState(FixedTurret* enemy, Player* player) {
	Initialize(enemy, player);
}

void FixedTurretWaitState::Initialize(FixedTurret* enemy, Player* player) {
	enemy_ = enemy;
	player_ = player;
	shotCoolTime_ = kShotCoolTime;
}

void FixedTurretWaitState::Update(FixedTurret* enemy) {
	// 機能停止状態なら早期リターン
	if (!enemy->GetIsActive()) { return; }

	float dist = Length(player_->GetWorldPosition() - enemy->GetWorldPosition());
	if (dist <= kShotModeRange) {
		if (shotCoolTime_ <= 0) {
			enemy->ChangeState(new FixedTurretAttackState(enemy_, player_));
		}
	}
	shotCoolTime_--;
}
