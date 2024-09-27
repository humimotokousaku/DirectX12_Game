#include "TitanBeamAttackState.h"
#include "TitanWeakPointState.h"
#include "Titan.h" 

TitanBeamAttackState::TitanBeamAttackState(Titan* titan, Player* player) {
	titan_ = titan;
	player_ = player;
	currentFrame_ = 0;
}

void TitanBeamAttackState::Initialize() {
	// パーツの状態変更
	titan_->GetPartsManager()->StartBeamAttack();
}

void TitanBeamAttackState::Update() {
	if (currentFrame_ >= kBeamFrame && titan_->GetPartsManager()->GetIsStateChangeable()) {
		// パーツの状態変更
		titan_->GetPartsManager()->StartRevealWeakPoint();
		// 弱点露出状態に変更
		titan_->ChangeState(new TitanWeakPointState(titan_, player_));

		return;
	}
	currentFrame_++;
}