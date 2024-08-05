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
	float dist = Length(player_->GetWorldPosition() - enemy->GetWorldPosition());
	if (dist <= kShotModeRange) {
		if (shotCoolTime_ <= 0) {
			enemy->ChangeState(new FixedTurretAttackState(enemy_, player_));
		}
	}
	shotCoolTime_--;

	//shotCoolTime_ = std::clamp<float>(shotCoolTime_, 0, kShotCoolTime);
}
