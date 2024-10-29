#pragma once
#include "IBeamEnemyState.h"

class BeamEnemy; // 前方宣言
class BeamEnemyStateHorizontal : public IBeamEnemyState {
public:
	BeamEnemyStateHorizontal(BeamEnemy* enemy, Player* player);
	~BeamEnemyStateHorizontal()override = default;
	
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