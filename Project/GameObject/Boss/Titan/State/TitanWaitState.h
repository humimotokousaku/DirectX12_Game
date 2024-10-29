#pragma once
#include "ITitanState.h"

class Titan; // 前方宣言
class TitanWaitState : public ITitanState {
public:
	enum AttackList {
		HandAttack,
		BeamAttack
	};

public:
	TitanWaitState(Titan* titan, Player* player);
	~TitanWaitState()override = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="titan">敵クラス</param>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="enemy">敵クラス</param>
	void Update()override;

	///
	/// User Method
	/// 

	// ランダムで攻撃パターンを選択する
	void RandomAttackSelector();

public:// 定数
	// 待機フレーム
	const int kWaitFrame = 240;

public:
	Titan* titan_;

	// 選択された攻撃パターン
	int selectedAttackPattern_ = -1;
};