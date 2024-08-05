#pragma once
#include "IEnemyState.h"
#include "TimedCall.h"

class Enemy; // 前方宣言
class EnemyStateWait : public IEnemyState {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="enemy">敵クラス</param>
	void Initialize(Enemy* enemy, Player* player);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="enemy">敵クラス</param>
	void Update(Enemy* enemy);

public:
	Enemy* enemy_;
};