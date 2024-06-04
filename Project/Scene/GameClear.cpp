#include "GameClear.h"
#include "GameManager.h"

void GameClear::Initialize() {
	sceneNum = GAMECLEAR_SCENE;
}

void GameClear::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		sceneNum = TITLE_SCENE;
	}

	ImGui::Begin("Current Scene");
	ImGui::Text("GAMECLEAR");
	ImGui::Text("SPACE:scene change");
	ImGui::End();
}

void GameClear::Draw() {

}

void GameClear::Finalize() {

}