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
	mpBehaviorTree->SelectLoadFile("behavior_tree.json");
	//auto behavior_tree_file_path = mpBehaviorTree->GetLoadingFile();

	mWorld.add_actor(new Player());
	mWorld.add_actor(new Attacker(&mWorld, camera_.get(), ""));
	//mWorld.add_actor(new Attacker(&mWorld, camera_.get(), behavior_tree_file_path));
}

void TitleScene::Update() {
	float delta_time = 1.f;
	mWorld.update(delta_time);

	//// 現在動かしているノードのIDを取得
	//const auto& attacker_actor = mWorld.find_actor("Attacker");
	//const Attacker* attacker = dynamic_cast<const Attacker*>(attacker_actor);
	//mRunnningNodeId = attacker->GetBTRunningNodeID();

	//mpBehaviorTree->SetRunnningNodeID(mRunnningNodeId);

	//// ツリーエディターの更新
	mpBehaviorTree->Update();
}

void TitleScene::Draw() {
	mWorld.draw();
	mpBehaviorTree->Draw();
}

void TitleScene::Finalize() {
	mWorld.clear();
	delete mpBehaviorTree;
}