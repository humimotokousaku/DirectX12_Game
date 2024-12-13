#pragma once
#include "BaseMoveEnemyState.h"
#include "TimedCall.h"

class MoveEnemy;
/// <summary>
/// 移動する敵の待機状態
/// </summary>
class MoveEnemyStateWait : public BaseMoveEnemyState {
public:
	~MoveEnemyStateWait()override = default;

	// 初期化
	void Initialize(MoveEnemy* enemy, Player* player);

	// 更新処理
	void Update(MoveEnemy* enemy);

public:
	MoveEnemy* enemy_;
};