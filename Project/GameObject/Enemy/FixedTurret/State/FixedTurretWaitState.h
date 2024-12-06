#pragma once
#include "IFixedTurretState.h"
#include "TimedCall.h"
#include <algorithm>

class FixedTurret;
/// <summary>
/// 弾を撃つ敵の待機状態
/// </summary>
class FixedTurretWaitState : public IFixedTurretState {
public:
	FixedTurretWaitState(FixedTurret* enemy, Player* player);
	~FixedTurretWaitState()override = default;

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
	const float kShotModeRange = 1000.0f;

public:
	FixedTurret* enemy_;
	// 射撃のクールタイム
	float shotCoolTime_;
};