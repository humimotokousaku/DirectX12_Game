#include "TitleScene.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
#include "GlobalVariables.h"
#include "Lerp.h"
#include "Audio.h"
#include "SceneTransition/SceneTransition.h"

void TitleScene::Initialize() {
	sceneNum = TITLE_SCENE;
	input_ = Input::GetInstance();
}

void TitleScene::Update() {
	if (input_->TriggerKey(DIK_SPACE) || input_->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		SceneTransition::GetInstance()->Start();
	}

	if (SceneTransition::GetInstance()->GetSceneTransitionSignal()) {
		sceneNum = GAME_SCENE;
	}
}

void TitleScene::Draw() {
	
}

void TitleScene::Finalize() {
	// 基底クラスの解放
	IScene::Finalize();
}