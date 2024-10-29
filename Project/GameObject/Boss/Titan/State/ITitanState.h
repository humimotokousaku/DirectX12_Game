#pragma once
#include "Player.h"

class Titan;
class ITitanState {
public:
	// 純粋仮想関数
	virtual ~ITitanState() = default;
	virtual void Initialize() = 0;
	virtual void Update() = 0;

public:
	Player* player_;

	// 経過時間
	int currentFrame_;
};