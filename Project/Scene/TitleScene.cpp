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

	// UIのスプライトを作成
	guideUI_[0] = std::make_unique<Sprite>();
	guideUI_[0]->Initialize("Level", "gray.png");
	guideUI_[0]->SetSize(Vector2{ 1280,720 });
	guideUI_[0]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 2 });
	guideUI_[1] = std::make_unique<Sprite>();
	guideUI_[1]->Initialize("Textures/UI", "titleName.png");
	guideUI_[1]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 });
	guideUI_[2] = std::make_unique<Sprite>();
	guideUI_[2]->Initialize("Textures/UI", "guide_pad_A.png");
	guideUI_[2]->SetSize(Vector2{ 64,64 });
	guideUI_[2]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 * 3 });
	for (int i = 0; i < 3; i++) {
		PostEffectManager::GetInstance()->AddSpriteList(guideUI_[i].get());
	}
}

void TitleScene::Update() {
	if (input_->TriggerKey(DIK_SPACE) || input_->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		SceneTransition::GetInstance()->Start();
	}
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_2)) {
		sceneNum = GAMEOVER_SCENE;
		SceneTransition::GetInstance()->Start();
	}
	if (input_->TriggerKey(DIK_3)) {
		sceneNum = GAMECLEAR_SCENE;
		SceneTransition::GetInstance()->Start();
	}
#endif

	if (SceneTransition::GetInstance()->GetSceneTransitionSignal()) {
		sceneNum = GAME_SCENE;
	}
}

void TitleScene::Draw() {
	// UI
	for (int i = 0; i < 3; i++) {
		guideUI_[i]->Draw();
	}
}

void TitleScene::Finalize() {
	// 基底クラスの解放
	IScene::Finalize();
}