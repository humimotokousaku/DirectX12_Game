#include "BehaviourTreeBulider.h"

#include "Actor/BlackBoard.h"
#include "INode.h"
#include "CompositeNode/Selector.h"
#include "CompositeNode/Sequence.h"
#include "DecoratorNode/Inverter.h"
#include "BranchNode/CheckNearPlayer.h"
#include "BranchNode/CheckFarPlayer.h"
#include "LeafNode/WaitLeaf.h"
#include "LeafNode/ChasePlayerLeaf.h"
#include "LeafNode/EscapeFromPlayerLeaf.h"
#include "LeafNode/CircleAttackLeaf.h"
#include "LeafNode/AlwaysSuccessLeaf.h"
#include "LeafNode/AlwaysFailLeaf.h"
#include "LeafNode/DebugDrawLeaf.h"

#include "../externals/nlohmann/json.hpp"
using json = nlohmann::json;

#include <fstream>

INode* BehaviourTreeBuilder::BuildAttackerTree(std::string file_path, BlackBoard* blackboard) {
	// ファイルストリームを作成
	std::ifstream file;

	// ファイルを開く
	file.open(file_path);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	json j;
	file >> j;

	// IDからJSON要素へのマッピングを構築する
	std::unordered_map<int, json> nodeMap;
	for (auto& node : j)
	{
		int id = node["id"].get<int>();
		nodeMap[id] = node;
	}

	// 再帰的にノードを構築するラムダ関数
	std::function<INode* (int)> buildNode = [&](int nodeId) -> INode*
		{
			INode* node = nullptr;

			// branchノード生成時に子ノードを設定していないと読み込みエラーになるので対策する
			if (nodeId == -1) { return node; }

			// ノードが存在しない場合
			if (nodeMap.find(nodeId) == nodeMap.end()) {
				throw std::runtime_error("Node id not found in JSON: " + std::to_string(nodeId));
			}
			auto nodeJson = nodeMap[nodeId];
			std::string name = nodeJson["name"].get<std::string>();

			// --- Compositeノード ---
			if (name == "Sequence") {
				Sequence* seq = new Sequence(blackboard);
				for (auto childId : nodeJson["children"]) {
					seq->AddNode(buildNode(childId.get<int>()));
				}
				node = seq;
			}
			else if (name == "Selector") {
				Selector* selector = new Selector(blackboard);
				for (auto childId : nodeJson["children"]) {
					selector->AddNode(buildNode(childId.get<int>()));
				}
				node = selector;
			}

			// --- Decoratorノード ---
			else if (name == "Inverter") {
				int childId = nodeJson["children"][0].get<int>();
				node = new Inverter(blackboard, buildNode(childId));
			}

			// --- Branchノード ---
			else if (name == "CheckNearPlayer") {
				int trueChildId = nodeJson["true_child"].get<int>();
				int falseChildId = nodeJson["false_child"].get<int>();
				float limitDistance = nodeJson["limit_distance"].get<float>();
				node = new CheckNearPlayer(blackboard, buildNode(trueChildId), buildNode(falseChildId), limitDistance);
			}
			else if (name == "CheckFarPlayer") {
				int trueChildId = nodeJson["true_child"].get<int>();
				int falseChildId = nodeJson["false_child"].get<int>();
				float limitDistance = nodeJson["limit_distance"].get<float>();
				node = new CheckFarPlayer(blackboard, buildNode(trueChildId), buildNode(falseChildId), limitDistance);
			}

			// --- Leafノード ---
			else if (name == "WaitLeaf") {
				float waitTime = nodeJson["wait_time"].get<float>();
				node = new WaitLeaf(blackboard, waitTime);
			}
			else if (name == "ChasePlayerLeaf") {
				node = new ChasePlayerLeaf(blackboard);
			}
			else if (name == "EscapeFromPlayerLeaf") {
				node = new EscapeFromPlayerLeaf(blackboard);
			}
			else if (name == "CircleAttackLeaf") {
				node = new CircleAttackLeaf(blackboard);
			}
			else if (name == "AlwaysSuccessLeaf") {
				node = new AlwaysSuccessLeaf(blackboard);
			}
			else if (name == "AlwaysFailLeaf") {
				node = new AlwaysFailLeaf(blackboard);
			}
			else if (name == "DebugDrawLeaf") {
				int text = nodeJson["text"].get<int>();
				node = new DebugDrawLeaf(blackboard, text);
			}
			// --- 未知のノード ---
			else {
				throw std::runtime_error("Unknown node type: " + name);
			}

			// すべてのノードにIDを設定
			if (node) {
				node->SetNodeID(nodeId);
			}

			return node;
		};

	// ルートノードを探す (parentが-1のノード)
	int rootId = -1;
	for (auto& [id, nodeJson] : nodeMap) {
		if (nodeJson["parent"].get<int>() == -1) {
			rootId = id;
			break;
		}
	}
	if (rootId == -1) {
		throw std::runtime_error("Root node not found in JSON");
	}

	return buildNode(rootId);
}
