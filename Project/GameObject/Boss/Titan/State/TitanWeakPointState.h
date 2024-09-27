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
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="titan">敵クラス</param>
	void Update()override;

private:// 定数
	// 弱点を露出する時間
	const int kWeakPointFrame = 300;

public:
	Titan* titan_;
};