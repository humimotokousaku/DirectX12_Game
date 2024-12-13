#include "MoveEnemyStateApproach.h"
#include "MoveEnemyStateLeave.h"
#include "MoveEnemy.h"
#include "Vector3.h"
#include "Lerp.h"

MoveEnemyStateApproach::~MoveEnemyStateApproach() {
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
}

void MoveEnemyStateApproach::FireAndResetTimer() {
	// 弾を発射する
	enemy_->Fire();
	// 発射タイマーをセットする
	timedCalls_.push_back(
		new TimedCall(std::bind(&MoveEnemyStateApproach::FireAndResetTimer, this), kFireInterval));
}

void MoveEnemyStateApproach::Initialize(MoveEnemy* enemy, Player* player) {
	enemy_ = enemy;
	player_ = player;
	//FireAndResetTimer();
}

void MoveEnemyStateApproach::Update(MoveEnemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = { 0, 0, 0.2f };

	// 移動処理
	enemy->Move(enemy->GetMoveSpeed());

	// 終了したタイマーを削除
	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished()) {
			delete timedCall;
			return true;
		}
		return false;
	});
	// 範囲forでリストの全要素について回す
	for (TimedCall* timedCall : timedCalls_) {
		timedCall->Update();
	}
}