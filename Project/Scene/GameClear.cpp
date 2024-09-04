#include "GameClear.h"
#include "GameManager.h"
#include "SceneTransition/SceneTransition.h"

void GameClear::Initialize() {
	sceneNum = GAMECLEAR_SCENE;
}

void GameClear::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		SceneTransition::GetInstance()->Start();
	}

	if (SceneTransition::GetInstance()->GetSceneTransitionSignal()) {
		sceneNum = TITLE_SCENE;
	}
}

void GameClear::Draw() {

}

void GameClear::Finalize() {
	// 基底クラスの解放
	IScene::Finalize();
}