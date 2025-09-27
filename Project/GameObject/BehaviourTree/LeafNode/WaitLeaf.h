#pragma once

#include "LeafNodeBase.h"

/// <summary>
/// 指定した時間待機する葉ノード
/// </summary>
class WaitLeaf : public LeafNodeBase {
public:
	explicit WaitLeaf(BlackBoard* black_board, const float wait_time);
	~WaitLeaf();

	void Tick() override;
	void Finalize() override;

private:
	float mWaitTime = 0.f;
	float mWaitCount = 0.f;
};