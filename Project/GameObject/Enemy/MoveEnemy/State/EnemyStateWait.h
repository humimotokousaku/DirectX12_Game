#pragma once
#include "IEnemyState.h"
#include "TimedCall.h"

class Enemy; // 前方宣言
class EnemyStateWait : public IEnemyState {
public:
	~EnemyStateWait()override = default;

	// 初期化
	void Initialize(Enemy* enemy, Player* player);

	// 更新処理
	void Update(Enemy* enemy);

public:
	Enemy* enemy_;
};