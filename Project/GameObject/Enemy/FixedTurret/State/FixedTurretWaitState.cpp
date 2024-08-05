#include "FixedTurretWaitState.h"
#include "FixedTurretAttackState.h"
#include "FixedTurret.h"

void FixedTurretWaitState::Initialize(FixedTurret* enemy, Player* player) {
	enemy_ = enemy;
	player_ = player;
}

void FixedTurretWaitState::Update(FixedTurret* enemy) {
	float dist = Length(player_->GetWorldPosition() - enemy->GetWorldPosition());
	if (dist <= kShotModeRange) {
		enemy->ChangeState(new FixedTurretAttackState());
	}
}
