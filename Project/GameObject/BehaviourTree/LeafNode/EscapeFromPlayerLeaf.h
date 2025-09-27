#pragma once

#include "LeafNodeBase.h"

/// <summary>
/// プレイヤーから逃げる葉ノード
/// </summary>
class EscapeFromPlayerLeaf : public LeafNodeBase {
public:
	explicit EscapeFromPlayerLeaf(BlackBoard* black_board);

	~EscapeFromPlayerLeaf();

	void Tick() override;

	NodeResult GetNodeResult() const override;
};