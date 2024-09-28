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
	// 既定の時間が過ぎる+パーツが状態変更可能なら待機状態に移行
	if (currentFrame_ >= kHandFrame && titan_->GetPartsManager()->GetIsStateChangeable()) {
		// パーツの状態変更
		titan_->GetPartsManager()->StartWait();
		// 待機状態に変更
		titan_->ChangeState(new TitanWaitState(titan_, player_));

		return;
	}
	currentFrame_++;
}