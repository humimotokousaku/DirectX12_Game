#pragma once
#include "NodeResult.h"

/// <summary>
/// ノードのインターフェース
/// </summary>
class INode {
public:
	// 仮想デストラクタ
	virtual ~INode() = default;
	// 初期化
	virtual void Init() = 0;
	// 動作
	virtual void Tick() = 0;
	// 後処理
	virtual void Finalize() = 0;
	// ノードの状態を取得
	virtual NodeResult GetNodeResult() const = 0;
	// ノードのIDを保存
	virtual void SetNodeID(const int id) = 0;
	// 現在動作中のノードのIDを取得
	virtual int GetRunningNodeID() const = 0;
};