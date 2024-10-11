#pragma once
#include "IBeamEnemyState.h"

class BeamEnemy; // 前方宣言
class BeamEnemyStateWait : public IBeamEnemyState {
public:
	BeamEnemyStateWait(BeamEnemy* enemy, Player* player);
	~BeamEnemyStateWait() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

public:

};