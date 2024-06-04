#include "GameScene.h"
#include "GameManager.h"

GameScene::GameScene() {
	int a = 1;
	a;
}

void GameScene::Initialize() {
	sceneNum = GAME_SCENE;
}

void GameScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		sceneNum = TITLE_SCENE;
	}
#ifdef USE_IMGUI

	ImGui::Begin("Current Scene");
	ImGui::Text("GAME");
	ImGui::Text("SPACE:scene change");
	ImGui::End();
#endif
}

void GameScene::Draw() {

}

void GameScene::Finalize() {

}