#pragma once
#include "Player.h"

class BeamEnemy;
/// <summary>
/// ビームを撃つ敵の状態の基底クラス
/// </summary>
class IBeamEnemyState {
public:
	// 純粋仮想関数
	virtual ~IBeamEnemyState() = default;
	virtual void Initialize() = 0;
	virtual void Update() = 0;

public:
	BeamEnemy* enemy_;
	Player* player_;
};