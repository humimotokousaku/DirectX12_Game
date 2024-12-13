#include "GameOver.h"
#include "GameManager.h"
#include "SceneTransition/SceneTransition.h"

void GameOver::Initialize() {
	sceneNum = GAMEOVER_SCENE;
}

void GameOver::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		SceneTransition::GetInstance()->Start();
	}

	if (SceneTransition::GetInstance()->GetSceneTransitionSignal()) {
		sceneNum = TITLE_SCENE;
	}
}

void GameOver::Draw() {

}

void GameOver::Finalize() {

}