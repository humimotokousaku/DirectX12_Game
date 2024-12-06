#include "TitleScene.h"
#include "Audio.h"
#include "GlobalVariables.h"
#include "ImGuiManager.h"
#include "Lerp.h"
#include "ModelManager.h"
#include "SceneTransition/SceneTransition.h"

void TitleScene::Initialize() {
	sceneNum = TITLE_SCENE;
	input_ = Input::GetInstance();

	titleEvent_ = std::make_unique<TitleEvent>();
	titleEvent_->Initialize();

	// UIのスプライトを作成
	guideUI_[0] = std::make_unique<Sprite>();
	guideUI_[0]->Initialize("Textures/UI", "titleName.png");
	guideUI_[0]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 });
	guideUI_[1] = std::make_unique<Sprite>();
	guideUI_[1]->Initialize("Textures/UI", "guide_pad_A.png");
	guideUI_[1]->SetSize(Vector2{ 64,64 });
	guideUI_[1]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 * 3 });
	for (int i = 0; i < guideUI_.size(); i++) {
		PostEffectManager::GetInstance()->AddSpriteList(guideUI_[i].get());
	}
}

void TitleScene::Update() {
	// シーン遷移演出開始
	if (input_->TriggerKey(DIK_SPACE) || input_->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		titleEvent_->SetIsActive(true);
		// UIを非表示にする
		for (int i = 0; i < guideUI_.size(); i++) {
			guideUI_[i]->isActive_ = false;
		}
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

	// タイトル演出
	titleEvent_->Update();

	// タイトル演出が終わっているならシーン遷移演出開始
	if(titleEvent_->GetIsEnd()){
		SceneTransition::GetInstance()->Start(); 
	}
	// シーン遷移演出が終わったら遷移する
	if (SceneTransition::GetInstance()->GetSceneTransitionSignal()) {
		sceneNum = GAME_SCENE;
	}
}

void TitleScene::Draw() {
	// タイトル演出
	titleEvent_->Draw();
}

void TitleScene::Finalize() {

}