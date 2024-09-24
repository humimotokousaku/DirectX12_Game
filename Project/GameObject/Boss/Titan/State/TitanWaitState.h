#pragma once
#include "ITitanState.h"
#include "TimedCall.h"

class Titan; // 前方宣言
class TitanWaitState : public ITitanState {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="titan">敵クラス</param>
	void Initialize(Titan* titan, Player* player);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="enemy">敵クラス</param>
	void Update(Titan* titan);

public:
	Titan* titan_;
};