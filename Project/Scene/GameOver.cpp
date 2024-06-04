#include "GameOver.h"
#include "GameManager.h"

void GameOver::Initialize() {
	sceneNum = GAMEOVER_SCENE;
}

void GameOver::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		sceneNum = TITLE_SCENE;
	}

#ifdef USE_IMGUI
	ImGui::Begin("Current Scene");
	ImGui::Text("GAMEOVER");
	ImGui::Text("SPACE:scene change");
	ImGui::End();
#endif
}

void GameOver::Draw() {

}

void GameOver::Finalize() {

}