#pragma once

#include "DecoratorNodeBase.h"

class Inverter : public DecoratorNodeBase {
public:
	// コンストラクタ
	explicit Inverter(BlackBoard* black_board, INode* child_node);
	// デストラクタ
	~Inverter();

	/// <summary>
	/// 更新
	/// </summary>
	void Tick() override;
};