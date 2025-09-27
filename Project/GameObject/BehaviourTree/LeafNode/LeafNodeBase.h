#pragma once

#include "../NodeBase.h"

/// <summary>
/// 葉ノードの基底クラス(Actorの具体的な行動処理)
/// </summary>
class LeafNodeBase : public NodeBase {
protected:
	explicit LeafNodeBase(BlackBoard* black_board) : NodeBase{ black_board } {}
	virtual ~LeafNodeBase() = default;
};