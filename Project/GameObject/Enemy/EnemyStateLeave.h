#pragma once
#include "IEnemyState.h"

class Enemy;

// 離脱フェーズのクラス
class EnemyStateLeave : public IEnemyState {
public:
		// 初期化
	void Initialize(Enemy* enemy);

	// 更新処理
	void Update(Enemy* enemy);

public:
	Enemy* enemy_;
};