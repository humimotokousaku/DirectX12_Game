#pragma once
#include "Player.h"

class FixedTurret; // 前方宣言
class IFixedTurretState {
public:
	// 純粋仮想関数
	virtual ~IFixedTurretState() = default;
	virtual void Initialize(FixedTurret* enemy, Player* player) = 0;
	virtual void Update(FixedTurret* enemy) = 0;

public:
	Player* player_;
};