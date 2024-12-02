#pragma once
#include "IFixedTurretState.h"
#include "TimedCall.h"
#include "Animation.h"
#include <algorithm>

class FixedTurret;
class FixedTurretLeaveState : public IFixedTurretState {
public:
	FixedTurretLeaveState(FixedTurret* enemy, Player* player);
	~FixedTurretLeaveState()override = default;

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
	// 離脱速度
	const Vector3 kMaxLeaveVelocity = { 0.0f, 20.0f,0.0f };

public:
	FixedTurret* enemy_;
	// 離脱アニメーション
	Animation leaveAnim_;

	// 離脱時の速度
	Vector3 leaveVel_;
};