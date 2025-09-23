#pragma once

#include "DecoratorNodeBase.h"

class Inverter : public DecoratorNodeBase {
public:
	explicit Inverter(BlackBoard* black_board, INode* child_node);

	~Inverter();

	void tick() override;
};