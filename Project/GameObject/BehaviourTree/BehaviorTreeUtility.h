#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <../externals/nlohmann/json.hpp>
#include "../externals/nameof/nameof.hpp"
#include "../externals/ImGui/imgui.h"


/// <summary>
/// ノードの種類
/// </summary>
enum class NodeType {
	Composite,
	Decorator,
	Branch,
	Leaf
};

/// <summary>
/// ノードの名前
/// </summary>
enum class NodeName {
	Sequence,
	Selector,
	Inverter,
	CheckFarPlayer,
	CheckNearPlayer,
	ChasePlayerLeaf,
	CircleAttackLeaf,
	DebugDrawLeef,
	AlwaysFailLeaf,
	AlwaysSuccessLeaf,
	EscapeFromPlayerLeaf,
	WaitLeaf,
	NameEnd, // 要素数取得用
};

/// <summary>
/// 各ノードの名前とタイプのマッピング
/// </summary>
static std::map<NodeName, NodeType> NODE_MAP = {
	{NodeName::Sequence, NodeType::Composite},
	{NodeName::Selector, NodeType::Composite},
	{NodeName::Inverter, NodeType::Decorator},
	{NodeName::CheckFarPlayer, NodeType::Branch},
	{NodeName::CheckNearPlayer, NodeType::Branch},
	{NodeName::ChasePlayerLeaf, NodeType::Leaf},
	{NodeName::CircleAttackLeaf, NodeType::Leaf},
	{NodeName::DebugDrawLeef, NodeType::Leaf},
	{NodeName::AlwaysFailLeaf, NodeType::Leaf},
	{NodeName::AlwaysSuccessLeaf, NodeType::Leaf},
	{NodeName::EscapeFromPlayerLeaf, NodeType::Leaf},
	{NodeName::WaitLeaf, NodeType::Leaf},
};

/// <summary>
/// ノードの構造体
/// </summary>
struct BTNode {
	int id = 0;                             // 固有ID
	NodeType type = NodeType::Leaf;         // ノードのタイプ
	NodeName name = NodeName::WaitLeaf;     // ノードの名前
	std::vector<int> children;              // 子ノードID
	int parent = -1;                        // 親ノードID

	// Branch用の変数
	int true_child = -1;
	int false_child = -1;

	// Wait用の変数
	float wait_time = -1.f;

	// CheckFarPlayer, CheckNearPlayer用の変数
	float limit_distance = -1.f;

	// エディター上での位置
	float pos_x = 0.f;
	float pos_y = 0.f;

	// 名前をstring型で取得
	std::string GetString() const {
		return std::string(NAMEOF_ENUM(name));
	}
};


/// <summary>
/// ノードで構成された木の構造体
/// </summary>
class BehaviorTreeGraph
{
	using json = nlohmann::json;

private:
	// 各種ノードタイプのタイトルバーの色
	const std::map<NodeType, ImU32> cNodeColors = {
		{ NodeType::Composite, IM_COL32(128, 32, 32, 255)},
		{ NodeType::Decorator, IM_COL32(32, 32, 128, 255) },
		{ NodeType::Branch,    IM_COL32(128, 128, 32, 255)},
		{ NodeType::Leaf,      IM_COL32(32, 128, 32, 255)}
	};

	const ImU32 cRunningColor = IM_COL32(250, 50, 0, 255);
	const ImU32 cLinkColor = IM_COL32(75, 75, 200, 200);

	const int cInputBit = 16;
	const int cTruePinBit = 1;
	const int cFalsePinBit = 2;

	const enum class NodeTuple {
		Node_Id,
		Child_Id,
		PinType,
	};

public:
	BehaviorTreeGraph(bool is_edit_mode);

	~BehaviorTreeGraph();

	// 初期化
	void Init();

	// 更新
	void Update();

	// 描画
	void Draw();

	// モード切替
	void ChangeMode(bool is_edit_mode);

	// ロードするJsonファイルを選択
	//※使ったらモデルの読み込み部分でバグ起きるので使用禁止
	void SelectLoadFile(const std::string& fileName);

	// 現在ロードしているJsonファイルパスを取得
	std::string GetLoadingFile() const { return mLoadFileName; }

	// 現在動かしているノード番号を設定
	void SetRunnningNodeID(const int running_node_id);

private:
	// ノードの追加
	int AddNode(NodeName name);

	/// <summary>
	/// リンクの追加(ノードごとに設定事項が違う可能性あり)
	/// </summary>
	/// <param name="start_attr"></param>
	/// <param name="end_attr"></param>
	void AddLink(int start_attr, int end_attr);

	// Json書き出し処理
	void ExportJson(const std::string& file_name);

	// Json読み込み処理
	void ImportJson(const std::string& file_name);

	// ノードの位置を更新
	void SetNodePos(int id, float x, float y);

	// 選択している要素を削除
	void DeleteSelectedItems();

private:
	// ノードのリンクを取得
	const std::unordered_map<int, std::tuple<int, int, int>>& GetNodeLinks() const { return mNodeLinks; }

	// ノード群を取得
	const std::unordered_map<int, BTNode>& GetNodes() const { return mNodes; }

	// 文字列からNodeNameを返す
	NodeName GetMatchingNodeName(std::string name);

	// 選択しているノードの数を取得
	const int GetSelectedNodeNum() const { return static_cast<int>(mSelectedNodes.size()); }

	// 選択しているノードを取得
	const int GetSelectedNode(int index);

	// 選択しているリンクの数を取得
	const int GetSelectedLinkNum() const { return static_cast<int>(mSelectedLinks.size()); }

private:
	// 選択している要素の更新
	void UpdateSelected();

	// ノードのリンクを行う
	void UpdateLinks();

	// ノードの移動処理
	void UpdateNodePos();

	// キーの更新処理
	void UpdateInputKey();

	// ツールバーの描画
	void DrawToolbar();

	// エディターの描画
	void DrawEditor();

private:
	// ノード追加ボタンの描画
	void DrawAddButton();

	// 書き出しボタンの描画
	void DrawExportButton();

	// 読み込みボタンの描画
	void DrawImportButton();

	// 削除ボタンの描画
	void DrawDeleteButton();

	// ノードを描画
	void DrawNodes();
	void DrawNode(const BTNode& node, int node_id, bool is_selected);

	// タイトルを描画
	void DrawTitle(const BTNode& node);

	// 入力ピンを描画
	void DrawInputPin(const BTNode& node);

	// 出力ピンを描画
	void DrawOutputPin(const BTNode& node, bool is_selected);

	/// <summary>
	/// パラメータを描画(ノードごとに設定事項が違う可能性あり)
	/// </summary>
	/// <param name="node"></param>
	/// <param name="node_id"></param>
	/// <param name="is_selected"></param>
	void DrawParameter(const BTNode& node, int node_id, bool is_selected);

	// リンクを描画
	void DrawLinks();

private:
	// ノードの削除
	void RemoveNode(int id);

	// ノードの削除
	void RemoveNode(const std::vector<int>& delete_list);

	/// <summary>
	/// ノードがリンクできるかどうか(ノードごとに設定事項が違う可能性あり)
	/// </summary>
	/// <param name="parent_node"></param>
	/// <param name="child_node"></param>
	/// <param name="is_true_branch"></param>
	/// <returns></returns>
	bool IsLinkAddable(BTNode& parent_node, BTNode& child_node, bool is_true_branch);

	/// <summary>
	/// ノード同士のリンクを解除(ノードごとに設定事項が違う可能性あり)
	/// </summary>
	/// <param name="parent_id"></param>
	/// <param name="child_id"></param>
	void RemoveNodesLink(int parent_id, int child_id);

	// ノードのリンクを削除
	void DeleteLink(int id);

	// ノードのリンクを削除
	void DeleteLink(const std::vector<int>& delete_list);

	// IDからノードを検索
	BTNode& GetNode(int id);

	// ノードのlimit_distanceを設定
	void SetLimitDistance(int id, float limit_distance);

	// ノードのwait_timeを設定
	void SetWaitTime(int id, float wait_time);

	// ノードのリンクを追加(NodeLinkだけ)
	void AddLinkTuple(int parent_id, int child_id, bool is_true_branch);

	// 全ノード名をEnumからStringの配列に変換
	std::vector<std::string> GetAllNodeNames() const;

	// 全選択を解除
	void ResetSelected();

	// 選択中のノードに関するリンクを洗い出す
	bool GetSelectedNodesRelatedLinks(std::vector<int>* links);

	// 渡されたノードに関するリンクを洗い出す
	bool GetNodesRelatedLinks(const int node_id, std::vector<int>* links, bool contain_child = true);

	// 渡されたノードに関するリンクを洗い出す(親まで含む)
	bool GetNodesRelatedAllLinks(const int node_id, std::vector<int>* links);

	// 渡されたノードに関するノードを洗い出す(親まで含む)
	bool GetNodesRelatedAllNodes(const int node_id, std::vector<int>* nodes);

	// 関連するリンクかどうか
	bool IsRelatedLinks(const int node_id, const std::pair<int, std::tuple<int, int, int>>& node_link, bool contain_child);

	/// <summary>
	/// 関連するノードかどうか(ノードごとに設定事項が違う部分あり)
	/// </summary>
	/// <param name="node_id"></param>
	/// <param name="node"></param>
	/// <returns></returns>
	bool IsRelatedNodes(const int node_id, const std::pair<const int, const BTNode>& node);

private:
	std::string mLoadFileName = "";

	// エディターモードにするか
	bool mIsEditMode = true;

	// ノード情報リスト
	std::unordered_map<int, BTNode>          mNodes;
	// リンクを描画するためのノードIDリスト
	// [0]->接続元ノードID
	// [1]->接続先子ノードID
	// [2]->branchノードのtrue,falseピンのどちらに接続しているかを識別するpin_type
	// →(branchノードでないなら全てfalseになる設計になってる)
	std::unordered_map<int, std::tuple<int, int, int>> mNodeLinks;

	// 生成されるリンクのID
	int mCreatedLinkId = 0;
	// 次のノード番号(ノード作成時の重複回避用)
	int mNextId = 1;

	// 選択されているノードID
	std::vector<int> mSelectedNodes;
	// 選択されているリンク
	std::vector<int> mSelectedLinks;

	// ノード追加のためのプレビューリスト
	std::vector<std::string> mNodeNames;
	// 追加予定のノードID
	int mSelectedAddNode = 0;

	// リアルタイム表示用
	std::vector<int> mRunningLinks;
	std::vector<int> mRunningNodes;
};