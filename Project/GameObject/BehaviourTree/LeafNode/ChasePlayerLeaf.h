#pragma once

#include "LeafNodeBase.h"

/// <summary>
/// プレイヤーを追いかける葉ノード
/// </summary>
class ChasePlayerLeaf : public LeafNodeBase {
public:
	explicit ChasePlayerLeaf(BlackBoard* black_board);

	~ChasePlayerLeaf();

	void Tick() override;

	NodeResult GetNodeResult() const override;
};