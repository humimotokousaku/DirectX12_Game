#include "GameClear.h"
#include "GameManager.h"

void GameClear::Initialize() {
	sceneNum = GAMECLEAR_SCENE;

	// UIのスプライトを作成
	guideUI_[0] = std::make_unique<Sprite>();
	guideUI_[0]->Initialize("", "guide_Clear.png");
	//guideUI_[0].reset(Sprite::Create("", "guide_Clear.png"));
	guideUI_[0]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 });
	guideUI_[1] = std::make_unique<Sprite>();
	guideUI_[1]->Initialize("", "guide_pad_A.png");
	//guideUI_[1].reset(Sprite::Create("", "guide_pad_A.png"));
	guideUI_[1]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 * 3 });
}

void GameClear::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		sceneNum = TITLE_SCENE;
	}
}

void GameClear::Draw() {
	// UI
	for (int i = 0; i < 2; i++) {
		guideUI_[i]->Draw();
	}
}

void GameClear::Finalize() {
	// 基底クラスの解放
	IScene::Finalize();
}