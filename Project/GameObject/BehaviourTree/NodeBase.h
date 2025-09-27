#pragma once

#include "INode.h"

class BlackBoard;

/// <summary>
/// 各ノードが継承する基底クラス
/// </summary>
class NodeBase : public INode {
protected:
	// コンストラクタ
	explicit NodeBase(BlackBoard* black_board) : mpBlackBoard{ black_board } {}
	// デストラクタ
	virtual ~NodeBase() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override {
		mNodeResult = NodeResult::Running;
	}
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Tick() override {}
	/// <summary>
	/// 解放処理
	/// </summary>
	virtual void Finalize() override {}

protected:// 継承先でのみ使用可能なアクセサ
	/// <summary>
	/// ノードの状態を取得
	/// </summary>
	/// <returns></returns>
	virtual NodeResult GetNodeResult() const {
		return mNodeResult;
	}
	/// <summary>
	/// 処理中のノードを取得
	/// </summary>
	/// <returns></returns>
	virtual int GetRunningNodeID() const override { return mNodeId; }

public:// パブリックなアクセサ
	/// <summary>
	/// ノードのIDを設定
	/// </summary>
	/// <param name="id"></param>
	void SetNodeID(const int id) override { mNodeId = id; }

protected:
	NodeResult mNodeResult = NodeResult::Idle; // ノードの状態
	BlackBoard* mpBlackBoard = nullptr; // ブラックボード

private:
	int mNodeId = -1;
};