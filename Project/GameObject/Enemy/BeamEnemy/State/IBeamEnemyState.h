#pragma once
#include "Player.h"

class BeamEnemy;
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