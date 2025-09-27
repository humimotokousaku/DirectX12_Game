#include "DebugDrawLeaf.h"

//#include "DxLib.h"

DebugDrawLeaf::DebugDrawLeaf(BlackBoard* black_board, int text)
	: LeafNodeBase(black_board),
	mText(text)
{

}

DebugDrawLeaf::~DebugDrawLeaf()
{
}

void DebugDrawLeaf::Tick()
{
	//printfDx("%i\n", mText);
}

NodeResult DebugDrawLeaf::GetNodeResult() const
{
	// 必ず成功を返す
	return NodeResult::Success;
}