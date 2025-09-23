#include "TitleScene.h"
#include "Audio.h"
#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include "Lerp.h"
#include "ModelManager.h"
#include "SceneTransition/SceneTransition.h"

#include "BehaviourTree/Actor/Enemy/Attacker.h"
#include "Player/Player.h"

void TitleScene::Initialize() {
	sceneNum = TITLE_SCENE;
	input_ = Input::GetInstance();

	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	mWorld.add_actor(new Player());
	mWorld.add_actor(new Attacker(&mWorld, camera_.get(),"Engine/resources/behavior_tree.json"));
}

void TitleScene::Update() {
	float delta_time = 1.f;
	mWorld.update(delta_time);
}

void TitleScene::Draw() {
	mWorld.draw();
}

void TitleScene::Finalize() {
	mWorld.clear();
}