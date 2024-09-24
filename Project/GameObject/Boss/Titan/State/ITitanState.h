#pragma once
#include "Player.h"

class Titan;
class ITitanState {
public:
	// 純粋仮想関数
	virtual void Initialize(Titan* titan, Player* player) = 0;
	virtual void Update(Titan* titan) = 0;

public:
	Player* player_;
};