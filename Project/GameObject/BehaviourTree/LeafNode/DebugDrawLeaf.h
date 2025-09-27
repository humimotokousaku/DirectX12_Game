#pragma once

#include <string>
#include "LeafNodeBase.h"

/// <summary>
/// 文字のデバッグ描画を行う葉ノード
/// </summary>
class DebugDrawLeaf : public LeafNodeBase {
public:
	explicit DebugDrawLeaf(BlackBoard* black_board, int text);

	~DebugDrawLeaf();

	void Tick() override;

	NodeResult GetNodeResult() const override;

private:
	int mText;
};