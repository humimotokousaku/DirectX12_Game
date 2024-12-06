#pragma once
#include "IEnemyState.h"

class Enemy;
/// <summary>
/// 移動する敵の離脱状態
/// </summary>
class EnemyStateLeave : public IEnemyState {
public:
	~EnemyStateLeave()override = default;

	// 初期化
	void Initialize(Enemy* enemy, Player* player);

	// 更新処理
	void Update(Enemy* enemy);

public:
	Enemy* enemy_;
};