#include "CompositeNodeBase.h"

CompositeNodeBase::~CompositeNodeBase()
{
	for (auto node : mChildNodes) {
		delete node;
	}
	mChildNodes.clear();
}

void CompositeNodeBase::Init()
{
	NodeBase::Init();
	mRunningNodeIndex = 0;

	// 最初のノードを初期化
	if (mChildNodes.size() > 0) {
		mChildNodes[mRunningNodeIndex]->Init();
	}
	else {
		mNodeResult = NodeResult::Fail;
	}
}

void CompositeNodeBase::Finalize()
{
	NodeBase::Finalize();

	// 最初のノードを初期化
	mRunningNodeIndex = 0;
	mChildNodes[mRunningNodeIndex]->Init();
}

void CompositeNodeBase::AddNode(INode* node)
{
	mChildNodes.push_back(node);
}

int CompositeNodeBase::GetRunningNodeID() const
{
	// 現在動かしているノードのIDを返す
	return mChildNodes[mRunningNodeIndex]->GetRunningNodeID();
}
