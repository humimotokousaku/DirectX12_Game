#pragma once

#include "BranchNodeBase.h"

class CheckNearPlayer : public BranchNodeBase {
public:
	// コンストラクタ
	explicit CheckNearPlayer(BlackBoard* black_board, INode* true_node, INode* false_node, const float max_distance);
	// デストラクタ
	~CheckNearPlayer();

private:

	const bool IsCondition() override;

private:
	const float mMaxDistance = 5;
};