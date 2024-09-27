#include "TitanHandAttackState.h"
#include "TitanWaitState.h"
#include "Titan.h" 

TitanHandAttackState::TitanHandAttackState(Titan* titan, Player* player) {
	titan_ = titan;
	player_ = player;
	currentFrame_ = 0;
}

void TitanHandAttackState::Initialize() {
	// パーツの状態変更
	titan_->GetPartsManager()->StartHandAttack();
}

void TitanHandAttackState::Update() {
	if (currentFrame_ >= kHandFrame && titan_->GetPartsManager()->GetIsStateChangeable()) {
		// パーツの状態変更
		titan_->GetPartsManager()->StartWait();
		// 待機状態に変更
		titan_->ChangeState(new TitanWaitState(titan_, player_));

		return;
	}
	currentFrame_++;
}