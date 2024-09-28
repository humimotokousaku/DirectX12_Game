#pragma once
#include "ITitanState.h"

class Titan;
class TitanHandAttackState : public ITitanState {
public:
	TitanHandAttackState(Titan* titan, Player* player);

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

public:// 定数
	// 待機フレーム
	const int kHandFrame = 300;

public:
	Titan* titan_;
};