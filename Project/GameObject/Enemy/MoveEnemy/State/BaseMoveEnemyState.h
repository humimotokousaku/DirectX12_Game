#pragma once
#include "Player.h"

class MoveEnemy;
/// <summary>
/// 移動する敵の状態の基底クラス
/// </summary>
class BaseMoveEnemyState {
public:
	// 純粋仮想関数
	virtual ~BaseMoveEnemyState() = default;
	virtual void Initialize(MoveEnemy* enemy, Player* player) = 0;
	virtual void Update(MoveEnemy* enemy) = 0;

public:
	Player* player_;
};