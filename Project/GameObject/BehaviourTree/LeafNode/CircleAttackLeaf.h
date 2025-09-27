#pragma once

#include "LeafNodeBase.h"

/// <summary>
/// 円形攻撃を行う葉ノード
/// </summary>
class CircleAttackLeaf : public LeafNodeBase {
public:
	explicit CircleAttackLeaf(BlackBoard* black_board);
	~CircleAttackLeaf();

	void Tick() override;

	NodeResult GetNodeResult() const override;
};