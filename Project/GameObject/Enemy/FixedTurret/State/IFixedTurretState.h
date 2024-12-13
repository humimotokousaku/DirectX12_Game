#pragma once
#include "Player.h"

class FixedTurret;
/// <summary>
/// 弾を撃つ敵の状態の基底クラス
/// </summary>
class IFixedTurretState {
public:
	// 純粋仮想関数
	virtual ~IFixedTurretState() = default;
	virtual void Initialize(FixedTurret* enemy, Player* player) = 0;
	virtual void Update(FixedTurret* enemy) = 0;

public:
	Player* player_;
};