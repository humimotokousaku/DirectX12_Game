#pragma once

class Player;
class PlayerBullet;

class BasePlayerBulletState {
public:
	// 純粋仮想関数
	virtual ~BasePlayerBulletState() = default;
	virtual void Initialize() = 0;
	virtual void Update() = 0;

public:
	Player* player_;
	PlayerBullet* playerBullet_;
};