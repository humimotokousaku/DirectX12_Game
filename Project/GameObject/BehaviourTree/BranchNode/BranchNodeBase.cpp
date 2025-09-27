#include "BranchNodeBase.h"

BranchNodeBase::BranchNodeBase(BlackBoard* black_board, INode* true_node, INode* false_node)
	: NodeBase(black_board)
{
	// [0]->trueノード
	// [1]->falseノード
	mpBranchNodes[0] = true_node;
	mpBranchNodes[1] = false_node;
}

BranchNodeBase::~BranchNodeBase()
{
	// ブランチノードの配列を解放
	for (int i = 0; i < 2; ++i) {
		if (mpBranchNodes[i] != nullptr) {
			delete mpBranchNodes[i];
			mpBranchNodes[i] = nullptr;
		}
	}
}

void BranchNodeBase::Init()
{
	NodeBase::Init();

	// 条件を満たしたか
	if (IsCondition()) {
		// trueノード
		mSatisfyIndex = 0;
	}
	else {
		// falseノード
		mSatisfyIndex = 1;
	}

	// 該当のノードの初期化
	mpBranchNodes[mSatisfyIndex]->Init();
}

void BranchNodeBase::Tick()
{
	mpBranchNodes[mSatisfyIndex]->Tick();
	// 現在のノード状態を更新
	mNodeResult = mpBranchNodes[mSatisfyIndex]->GetNodeResult();
}

void BranchNodeBase::Finalize()
{
	NodeBase::Finalize();
	mpBranchNodes[mSatisfyIndex]->Finalize();
	mSatisfyIndex = -1;
}

int BranchNodeBase::GetRunningNodeID() const
{
	return mpBranchNodes[mSatisfyIndex]->GetRunningNodeID();
}
