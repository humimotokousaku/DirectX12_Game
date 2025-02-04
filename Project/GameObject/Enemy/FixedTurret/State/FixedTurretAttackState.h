#pragma once
#include "BaseFixedTurretState.h"
#include "TimedCall.h"

class FixedTurret;
/// <summary>
/// 弾を撃つ敵の射撃状態
/// </summary>
class FixedTurretAttackState : public BaseFixedTurretState {
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
	static const int kFireInterval = 120;
	// 射撃のクールタイム
	const float kShotCoolTime = 120.0f;
	// 連続で射撃する回数
	const int kLimitShot = 1;

	// 自機を追尾する時間
	const float kMaxFollowTime = 180.0f;

public:
	FixedTurret* enemy_;
	// 時限発動
	std::list<TimedCall*> timedCalls_;
	// 発射回数
	int shotCount_;
	// 射撃のクールタイム
	float shotCoolTime_;

	// 経過フレーム
	float currentFrame_;
};