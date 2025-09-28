#pragma once

#include <string>

class INode;
class BlackBoard;

/// <summary>
/// 読みこんだツリーの組み立て(branchノードを使用する際必ずすべてのピンに何かしらのノードをつけないとエラーが起きます)
/// </summary>
class BehaviourTreeBuilder {
public:
	static INode* BuildAttackerTree(std::string file_path, BlackBoard* blackboard);
};