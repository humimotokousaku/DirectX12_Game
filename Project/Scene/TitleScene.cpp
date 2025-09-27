#include "TitleScene.h"
#include "Audio.h"
#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include "Lerp.h"
#include "ModelManager.h"
#include "SceneTransition/SceneTransition.h"

#include "BehaviourTree/Actor/Enemy/Attacker.h"
#include "Player/Player.h"

#include "../../externals/imnodes/imnodes.h"

void TitleScene::Initialize() {
	sceneNum = TITLE_SCENE;
	input_ = Input::GetInstance();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	mpBehaviorTree = new BehaviorTreeGraph(true);

	mWorld.add_actor(new Player());
	mWorld.add_actor(new Attacker(&mWorld, camera_.get(),"Engine/resources/behavior_tree.json"));
}

void TitleScene::Update() {
	float delta_time = 1.f;
	mWorld.update(delta_time);

	// ツリーエディターの更新
	mpBehaviorTree->update();

	//ImGui::Begin("Node Editor");
	//ImNodes::BeginNodeEditor();

	//for (int i = 0; i < 3; i++) {
	//	ImNodes::BeginNode(i);

	//	int j = 0;
	//	if (i > 0) { j = 3; }

	//	ImNodes::BeginInputAttribute(i + j);
	//	ImGui::Text("input");
	//	ImNodes::EndInputAttribute();

	//	ImNodes::BeginOutputAttribute(i + 1 + j);
	//	ImGui::Text("1 output");
	//	ImNodes::EndOutputAttribute();

	//	ImNodes::BeginOutputAttribute(i + 2 + j);
	//	ImGui::Text("2 output");
	//	ImNodes::EndOutputAttribute();
	//	ImNodes::EndNode();
	//}

	//ImNodes::MiniMap();

	//ImNodes::EndNodeEditor();
	//ImGui::End();
}

void TitleScene::Draw() {
	mWorld.draw();
	mpBehaviorTree->draw();
}

void TitleScene::Finalize() {
	mWorld.clear();
	delete mpBehaviorTree;
}