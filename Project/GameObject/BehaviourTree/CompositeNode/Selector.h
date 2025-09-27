#pragma once

#include <vector>
#include "CompositeNodeBase.h"

/// <summary>
/// 
/// </summary>
class Selector : public CompositeNodeBase {
public:
	// コンストラクタ
	explicit Selector(BlackBoard* black_board);
	// デストラクタ
	~Selector();

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