#pragma once

#include "../NodeBase.h"

class DecoratorNodeBase : public NodeBase {
public:
	explicit DecoratorNodeBase(BlackBoard* black_board) : NodeBase(black_board) {}
	virtual ~DecoratorNodeBase();

	virtual void Init() override;
	virtual void Finalize() override;

	void SetNode(INode* node);

	int GetRunningNodeID() const override;

protected:
	INode* mChildNode = nullptr;
};