#pragma once
#include "ITitanState.h"

class Titan;
class TitanWeakPointState : public ITitanState {
public:
	TitanWeakPointState(Titan* titan, Player* player);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="titan">敵クラス</param>
	void Initialize(Titan* titan, Player* player);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="titan">敵クラス</param>
	void Update(Titan* titan);

private:// 定数

public:
	Titan* titan_;
	// 射撃のクールタイム
	float shotCoolTime_;
};