#pragma once

class Enemy; // 前方宣言

// 基底クラス
class IEnemyState {
public:
	// 純粋仮想関数
	virtual void Initialize(Enemy* enemy) = 0;
	virtual void Update(Enemy* enemy) = 0;

public:
};