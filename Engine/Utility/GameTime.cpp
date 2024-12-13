#include "GameTime.h"
#include "ImGuiManager.h"

GameTimer* GameTimer::GetInstance() {
	static GameTimer instance;
	return &instance;
}

void GameTimer::Initialize() {

}

void GameTimer::Update() {
#ifdef _DEBUG
	ImGui::Begin("GameTimer");
	ImGui::DragFloat("Scale", &timeScale_, 0.01f, 0.0f, 2.0f);
	ImGui::End();
#endif // DEBUG
}
