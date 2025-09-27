#include "Selector.h"

Selector::Selector(BlackBoard* black_board)
	: CompositeNodeBase(black_board)
{

}

Selector::~Selector()
{
}

void Selector::Tick()
{
	mChildNodes[mRunningNodeIndex]->Tick();
	auto result = mChildNodes[mRunningNodeIndex]->GetNodeResult();

	if (result == NodeResult::Fail) {
		// 次回Sequenceに向けてノード番号を進める
		NodeIncrement();
		return;
	}

	// もし成功が返されたらノード終了
	if (result == NodeResult::Success) {
		Finalize();
	}

	mNodeResult = result;
}

const int Selector::GetNextIndex() const
{
	return mRunningNodeIndex + 1;
}

void Selector::NodeIncrement()
{
	// 現在のノードの後始末
	mChildNodes[mRunningNodeIndex]->Finalize();

	// インデックスを進める
	mRunningNodeIndex = GetNextIndex();

	// もしすべての子ノードを回していたら
	if (mRunningNodeIndex > mChildNodes.size() - 1) {
		mNodeResult = NodeResult::Fail;
		Finalize();
		return;
	}

	// 次に回すノードの初期化
	mChildNodes[mRunningNodeIndex]->Init();
}
