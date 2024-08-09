#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"
#include "Enemy.h"
#include "../math/Vector3.h"
#include "../math/Lerp.h"

EnemyStateApproach::~EnemyStateApproach() {
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
}

void EnemyStateApproach::FireAndResetTimer() {
	// 弾を発射する
	enemy_->Fire();
	// 発射タイマーをセットする
	timedCalls_.push_back(
		new TimedCall(std::bind(&EnemyStateApproach::FireAndResetTimer, this), kFireInterval));
}

void EnemyStateApproach::Initialize(Enemy* enemy, Player* player) {
	enemy_ = enemy;
	player_ = player;
	//FireAndResetTimer();
}

void EnemyStateApproach::Update(Enemy* enemy) {
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

float EnemyStateApproach::EaseOutBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1;

	return 1 + c3 * (float)pow(x - 1, 3) + c1 * (float)pow(x - 1, 2);
}