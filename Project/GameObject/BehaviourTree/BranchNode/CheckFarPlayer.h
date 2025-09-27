#pragma once

#include "BranchNodeBase.h"

class CheckFarPlayer : public BranchNodeBase {
public:
	explicit CheckFarPlayer(BlackBoard* black_board, INode* true_node, INode* false_node, const float max_distance);

	~CheckFarPlayer();

private:
	const bool IsCondition() override;

private:
	const float mMaxDistance = 5;
};