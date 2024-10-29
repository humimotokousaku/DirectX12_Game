#pragma once
#include "IFixedTurretState.h"
#include "TimedCall.h"

class FixedTurret; // 前方宣言

/// <summary>
/// 射撃状態のクラス
/// </summary>
class FixedTurretAttackState : public IFixedTurretState {
public:
	FixedTurretAttackState(FixedTurret* enemy, Player* player);
	~FixedTurretAttackState()override;

	/// <summary>
	/// 弾を発射してタイマーをリセット
	/// </summary>
	void FireAndResetTimer();

	// 初期化
	void Initialize(FixedTurret* enemy, Player* player);

	// 更新処理
	void Update(FixedTurret* enemy);

public:
	// 発射間隔
	static const int kFireInterval = 10;
	// 射撃のクールタイム
	const float kShotCoolTime = 90.0f;
	// 連続で射撃する回数
	const int kLimitShot = 3;

public:
	FixedTurret* enemy_;
	// 時限発動
	std::list<TimedCall*> timedCalls_;
	// 発射回数
	int shotCount_;
	// 射撃のクールタイム
	float shotCoolTime_;
};