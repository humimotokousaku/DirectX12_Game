#include "FixedTurretAttackState.h"
#include "FixedTurretLeaveState.h"
#include "FixedTurret.h"

FixedTurretAttackState::FixedTurretAttackState(FixedTurret* enemy, Player* player) {
	Initialize(enemy, player);
}
FixedTurretAttackState::~FixedTurretAttackState() {
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
	timedCalls_.clear();
}

void FixedTurretAttackState::FireAndResetTimer() {
	// 弾を発射する
	enemy_->Fire();
	// 発射タイマーをセットする
	timedCalls_.push_back(
		new TimedCall(std::bind(&FixedTurretAttackState::FireAndResetTimer, this), kFireInterval));

	// 射撃回数を更新
	shotCount_++;
}

void FixedTurretAttackState::Initialize(FixedTurret* enemy, Player* player) {
	enemy_ = enemy;
	player_ = player;
	shotCount_ = 0;
	currentFrame_ = kMaxFollowTime;
	FireAndResetTimer();
	// 機能停止ができない状態にする
	enemy->SetIsStopActive(false);
}

void FixedTurretAttackState::Update(FixedTurret* enemy) {
	// 自機に身体を向ける
	enemy->Aim();

	// 終了したタイマーを削除
	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished()) {
			delete timedCall;
			return true;
		}
		return false;
		});
	// 発射インターバル更新
	for (TimedCall* timedCall : timedCalls_) {
		timedCall->Update();
	}

	// 既定の回数撃ったらクールタイム状態に移行
	// もしくは機能停止状態なら待機状態に移行
	if (shotCount_ >= kLimitShot && currentFrame_ <= 0 || !enemy->GetIsActive()) {
		enemy->ChangeState(new FixedTurretLeaveState(enemy, player_));
	}

	currentFrame_--;
}