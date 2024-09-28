#include "TitanWaitState.h"
#include "TitanHandAttackState.h"
#include "TitanBeamAttackState.h"
#include "Titan.h" 
#include <random>

TitanWaitState::TitanWaitState(Titan* titan, Player* player) {
	titan_ = titan;
	player_ = player;
	selectedAttackPattern_ = -1;
	currentFrame_ = 0;
}

void TitanWaitState::Initialize() {
	// パーツの状態変更
	titan_->GetPartsManager()->StartWait();
}

void TitanWaitState::Update() {
	// 状態変更の条件
	if (currentFrame_ >= kWaitFrame && titan_->GetPartsManager()->GetIsStateChangeable()) {
		// 次の攻撃パターンをランダムで選択
		RandomAttackSelector();
		return;
	}
	currentFrame_++;
}

void TitanWaitState::RandomAttackSelector() {
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	std::uniform_real_distribution<float> distribution(0, 1);
	if (distribution(randomEngine) <= 1.0f) {
		selectedAttackPattern_ = HandAttack;
	}
	else {
		selectedAttackPattern_ = BeamAttack;
	}

	switch (selectedAttackPattern_) {
		/// 両手攻撃
	case HandAttack:
		// パーツの状態変更
		titan_->GetPartsManager()->StartHandAttack();
		AttackAlert::GetInstance()->Start();
		// 状態変更
		titan_->ChangeState(new TitanHandAttackState(titan_, player_));
		break;

		/// ビーム攻撃
	case BeamAttack:
		// パーツの状態変更
		titan_->GetPartsManager()->StartBeamAttack();
		// 状態変更
		titan_->ChangeState(new TitanBeamAttackState(titan_, player_));
		break;
	}
}
