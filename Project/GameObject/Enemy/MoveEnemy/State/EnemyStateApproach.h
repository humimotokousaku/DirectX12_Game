#pragma once
#include "IEnemyState.h"
#include "TimedCall.h"

class Enemy;
/// <summary>
/// 移動する敵の接近状態
/// </summary>
class EnemyStateApproach : public IEnemyState {
public:
	~EnemyStateApproach()override;

	/// <summary>
	/// 弾を発射してタイマーをリセット
	/// </summary>
	void FireAndResetTimer();

	// 初期化
	void Initialize(Enemy* enemy, Player* player);

	// 更新処理
	void Update(Enemy* enemy);

private:
	float EaseOutBack(float x);

public:
	Enemy* enemy_;
	// 発射間隔
	static const int kFireInterval = 60;
	// 時限発動
	std::list<TimedCall*> timedCalls_;
};