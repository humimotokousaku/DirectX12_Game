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

	ModelManager::GetInstance()->LoadModel("Models", "cube.obj");

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->worldTransform_.translate.y = 20.0f;
	camera_->worldTransform_.rotate.x = 3.14f / 2.0f;

	mpBehaviorTree = new BehaviorTreeGraph(true);
	mpBehaviorTree->SelectLoadFile("behavior_tree.json");
	//auto behavior_tree_file_path = mpBehaviorTree->GetLoadingFile();

	mWorld.add_actor(new Player(camera_.get()));
	mWorld.add_actor(new Attacker(&mWorld, camera_.get(), ""));
	//mWorld.add_actor(new Attacker(&mWorld, camera_.get(), behavior_tree_file_path));
}

void TitleScene::Update() {
	camera_->Update();

	float delta_time = 1.f;
	mWorld.update(delta_time);

	//// 現在動かしているノードのIDを取得
	//const auto& attacker_actor = mWorld.find_actor("Attacker");
	//const Attacker* attacker = dynamic_cast<const Attacker*>(attacker_actor);
	//mRunnningNodeId = attacker->GetBTRunningNodeID();

	//mpBehaviorTree->SetRunnningNodeID(mRunnningNodeId);

	//// ツリーエディターの更新
	mpBehaviorTree->Update();

	ImGui::Begin("GameTab");
	if (ImGui::TreeNode("Camera")) {		
		ImGui::DragFloat3("Translate", &camera_->worldTransform_.translate.x, 0.01f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Rotate", &camera_->worldTransform_.rotate.x, 0.01f, -6.28f, 6.28f);
		ImGui::TreePop();
	}
	ImGui::End();
}

void TitleScene::Draw() {
	mWorld.draw();
	mpBehaviorTree->Draw();
}

void TitleScene::Finalize() {
	mWorld.clear();
	delete mpBehaviorTree;
}