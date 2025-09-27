#pragma once

#include <vector>
#include "CompositeNodeBase.h"

class Sequence : public CompositeNodeBase {
public:
	// コンストラクタ
	explicit Sequence(BlackBoard* black_board);
	// デストラクタ
	~Sequence();

	/// <summary>
	/// 更新
	/// </summary>
	void Tick() override;

private:
	/// <summary>
	/// 次のID取得
	/// </summary>
	/// <returns></returns>
	const int GetNextIndex() const override;
	/// <summary>
	/// 次のノードに進む
	/// </summary>
	void NodeIncrement() override;
};