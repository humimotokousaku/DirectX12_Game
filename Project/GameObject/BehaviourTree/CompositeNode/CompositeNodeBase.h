#pragma once

#include <vector>
#include "../NodeBase.h"

/// <summary>
/// コンポジットノードの基底クラス
/// </summary>
class CompositeNodeBase : public NodeBase {
public:
	// コンストラクタ
	explicit CompositeNodeBase(BlackBoard* black_board) : NodeBase(black_board) {
	};
	// デストラクタ
	virtual ~CompositeNodeBase();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override;
	/// <summary>
	/// 解放処理
	/// </summary>
	virtual void Finalize() override;

protected:// 派生クラスで実装
	/// <summary>
	/// 次のID取得
	/// </summary>
	/// <returns></returns>
	virtual const int GetNextIndex() const = 0;
	/// <summary>
	/// 次のノードに進む
	/// </summary>
	virtual void NodeIncrement() = 0;

public:// アクセサ
	/// <summary>
	/// ノードの追加
	/// </summary>
	/// <param name="node"></param>
	void AddNode(INode* node);
	/// <summary>
	/// 現在動いている子ノードIDを取得
	/// </summary>
	/// <returns></returns>
	int GetRunningNodeID() const override;

protected:
	// 子ノード群
	std::vector<INode*> mChildNodes;
	// 現在動かしているノードのインデックス
	int mRunningNodeIndex{ 0 };
};