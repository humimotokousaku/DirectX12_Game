#include "FixedTurretWaitState.h"
#include "FixedTurretAttackState.h"
#include "FixedTurret.h"

FixedTurretWaitState::FixedTurretWaitState(FixedTurret* enemy, Player* player) {
	Initialize(enemy, player);
}

void FixedTurretWaitState::Initialize(FixedTurret* enemy, Player* player) {
	enemy_ = enemy;
	player_ = player;
}

void FixedTurretWaitState::Update(FixedTurret* enemy) {
	// 機能停止状態なら早期リターン
	if (!enemy->GetIsActive()) { return; }

	// 移動処理
	enemy->Move();

	// 射程範囲に自機がいるか
	float dist = Length(player_->GetWorldPosition() - enemy->GetWorldPosition());
	if (dist <= kShotModeRange) {
		// 移動中は弾を撃たない
		if (enemy->GetIsMove()) { return; }
		// 移動終了したら攻撃を開始する
		enemy->ChangeState(new FixedTurretAttackState(enemy_, player_));
	}
	shotCoolTime_--;
}
