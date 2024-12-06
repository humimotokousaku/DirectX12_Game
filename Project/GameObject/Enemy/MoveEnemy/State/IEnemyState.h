#pragma once
#include "Player.h"

class Enemy;
/// <summary>
/// 移動する敵の状態の基底クラス
/// </summary>
class IEnemyState {
public:
	// 純粋仮想関数
	virtual ~IEnemyState() = default;
	virtual void Initialize(Enemy* enemy, Player* player) = 0;
	virtual void Update(Enemy* enemy) = 0;

public:
	Player* player_;
};