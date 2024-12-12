#include "FixedTurretLeaveState.h"
#include "FixedTurret.h"

FixedTurretLeaveState::FixedTurretLeaveState(FixedTurret* enemy, Player* player) {
	Initialize(enemy, player);
}

void FixedTurretLeaveState::Initialize(FixedTurret* enemy, Player* player) {
	enemy_ = enemy;
	player_ = player;

	leaveAnim_.SetAnimData(&leaveVel_, Vector3{ 0,0,0 }, kMaxLeaveVelocity, 40, Easings::EaseInExpo);
	leaveAnim_.SetAnimData(&leaveVel_, Vector3{ 0,0,0 }, Vector3{0,0,0}, 180, Easings::EaseInExpo);
	leaveAnim_.SetIsStart(true);
}

void FixedTurretLeaveState::Update(FixedTurret* enemy) {
	// 機能停止状態なら早期リターン
	if (!enemy->GetIsActive()) { return; }
	
	// 離脱アニメーションの更新処理
	leaveAnim_.Update();

	// ローカル座標の変更
	Vector3 pos = enemy->GetLocalPosition() + leaveVel_;
	enemy_->SetLocalPosition(pos);

	// 離脱完了しているなら削除
	if (leaveAnim_.GetIsEnd()) {
		enemy->SetIsActive(false);
	}
}
