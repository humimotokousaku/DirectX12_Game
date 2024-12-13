#pragma once
#include "BaseMoveEnemyState.h"

class Enemy;
/// <summary>
/// 移動する敵の離脱状態
/// </summary>
class MoveEnemyStateLeave : public BaseMoveEnemyState {
public:
	~MoveEnemyStateLeave()override = default;

	// 初期化
	void Initialize(MoveEnemy* enemy, Player* player);

	// 更新処理
	void Update(MoveEnemy* enemy);

public:
	MoveEnemy* enemy_;
};