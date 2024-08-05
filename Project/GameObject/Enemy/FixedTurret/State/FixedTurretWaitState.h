#pragma once
#include "IFixedTurretState.h"
#include "TimedCall.h"
#include <algorithm>

class FixedTurret;
class FixedTurretWaitState : public IFixedTurretState {
public:
	FixedTurretWaitState(FixedTurret* enemy, Player* player);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="enemy">敵クラス</param>
	void Initialize(FixedTurret* enemy, Player* player);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="enemy">敵クラス</param>
	void Update(FixedTurret* enemy);

private:// 定数
	// 射撃状態になる範囲
	const float kShotModeRange = 100.0f;
	// 射撃のクールタイム
	const float kShotCoolTime = 90.0f;

public:
	FixedTurret* enemy_;
	// 射撃のクールタイム
	float shotCoolTime_;
};