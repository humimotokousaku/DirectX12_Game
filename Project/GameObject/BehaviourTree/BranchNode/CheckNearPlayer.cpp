#include "CheckNearPlayer.h"

#include "Vector2.h"
#include "../Actor/BlackBoard.h"
#include "../Actor/IAgent.h"

CheckNearPlayer::CheckNearPlayer(BlackBoard* black_board, INode* true_node, INode* false_node, const float max_distance)
	: BranchNodeBase(black_board, true_node, false_node),
	mMaxDistance(max_distance) {
}

CheckNearPlayer::~CheckNearPlayer()
{
}

const bool CheckNearPlayer::IsCondition()
{
	// プレイヤーの位置を取得
	auto player_pos = mpBlackBoard->get_value<Vector2>("PlayerPos");
	auto* agent = mpBlackBoard->get_value<IAgent*>("Agent");

	auto vector = player_pos - agent->get_position();
	auto result = std::sqrt((vector.x * vector.x) + (vector.y * vector.y));

	return result < mMaxDistance;
}