#pragma once

#include "../NodeBase.h"

/// <summary>
/// ブランチノードの基底クラス
/// </summary>
class BranchNodeBase : public NodeBase {
public:
	// コンストラクタ
	explicit BranchNodeBase(BlackBoard* black_board, INode* true_node, INode* false_node);
	// デストラクタ
	virtual ~BranchNodeBase();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Tick() override;
	/// <summary>
	/// 解放
	/// </summary>
	virtual void Finalize() override;

protected:
	/// <summary>
	/// 条件を満たしているか
	/// </summary>
	/// <returns></returns>
	virtual const bool IsCondition() = 0;

public:// アクセサ
	/// <summary>
	/// 現在動いているノードID取得
	/// </summary>
	/// <returns></returns>
	int GetRunningNodeID() const override;

protected:
	// [0]->trueノード
	// [1]->falseノード
	INode* mpBranchNodes[2] = { nullptr, nullptr }; // ブランチノードの配列
	int mSatisfyIndex = -1; // 条件を満たしているノードのインデックス
};
