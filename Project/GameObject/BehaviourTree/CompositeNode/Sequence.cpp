#include "Sequence.h"

Sequence::Sequence(BlackBoard* black_board)
	: CompositeNodeBase(black_board)
{

}

Sequence::~Sequence()
{
}

void Sequence::Tick()
{
	mChildNodes[mRunningNodeIndex]->Tick();
	auto result = mChildNodes[mRunningNodeIndex]->GetNodeResult();

	if (result == NodeResult::Success) {
		// 次回Sequenceに向けてノード番号を進める
		NodeIncrement();
		return;
	}

	// もし失敗が返されたらノード終了
	if (result == NodeResult::Fail) {
		Finalize();
	}

	mNodeResult = result;
}

const int Sequence::GetNextIndex() const
{
	return mRunningNodeIndex + 1;
}

void Sequence::NodeIncrement()
{
	// 現在のノードの後始末
	mChildNodes[mRunningNodeIndex]->Finalize();

	// インデックスを進める
	mRunningNodeIndex = GetNextIndex();

	// もしすべての子ノードを回していたら
	if (mRunningNodeIndex > mChildNodes.size() - 1) {
		mNodeResult = NodeResult::Success;
		Finalize();
		return;
	}

	// 次に回すノードの初期化
	mChildNodes[mRunningNodeIndex]->Init();
}
