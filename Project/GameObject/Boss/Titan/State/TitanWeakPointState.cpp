#include "TitanWeakPointState.h"
#include "TitanWaitState.h"
#include "Titan.h"

TitanWeakPointState::TitanWeakPointState(Titan* titan, Player* player) {
	titan_ = titan;
	player_ = player;
	currentFrame_ = 0;
}

void TitanWeakPointState::Initialize() {
	// パーツの状態変更
	titan_->GetPartsManager()->StartRevealWeakPoint();
}

void TitanWeakPointState::Update() {

	// 状態変更の条件
	if (currentFrame_ >= kWeakPointFrame && titan_->GetPartsManager()->GetIsStateChangeable()) {
		// パーツの状態変更
		titan_->GetPartsManager()->StartWait();
		// 待機状態に変更
		titan_->ChangeState(new TitanWaitState(titan_, player_));

		return;
	}
	currentFrame_++;
}
