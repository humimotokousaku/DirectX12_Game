#include "DecoratorNodeBase.h"

DecoratorNodeBase::~DecoratorNodeBase()
{
	if (mChildNode) {
		delete mChildNode;
		mChildNode = nullptr;
	}
}

void DecoratorNodeBase::Init()
{
	NodeBase::Init();
	mChildNode->Init();
}

void DecoratorNodeBase::Finalize()
{
	NodeBase::Finalize();
	mChildNode->Finalize();
}

void DecoratorNodeBase::SetNode(INode* node)
{
	mChildNode = node;
}

int DecoratorNodeBase::GetRunningNodeID() const
{
	return mChildNode->GetRunningNodeID();
}
