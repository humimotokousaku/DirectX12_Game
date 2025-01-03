#pragma once
#include "BaseMoveEnemyState.h"
#include "TimedCall.h"

class MoveEnemy;
/// <summary>
/// 移動する敵の接近状態
/// </summary>
class MoveEnemyStateApproach : public BaseMoveEnemyState {
public:
	~MoveEnemyStateApproach()override;

	/// <summary>
	/// 弾を発射してタイマーをリセット
	/// </summary>
	void FireAndResetTimer();

	// 初期化
	void Initialize(MoveEnemy* enemy, Player* player) override;

	// 更新処理
	void Update(MoveEnemy* enemy) override;

public:
	MoveEnemy* enemy_;
	// 発射間隔
	static const int kFireInterval = 60;
	// 時限発動
	std::list<TimedCall*> timedCalls_;
};