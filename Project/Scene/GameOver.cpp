#include "GameOver.h"
#include "GameManager.h"

void GameOver::Initialize() {
	sceneNum = GAMEOVER_SCENE;

	// UIのスプライトを作成
	guideUI_[0] = std::make_unique<Sprite>();
	guideUI_[0]->Initialize("", "guide_GameOver.png");
	guideUI_[0]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 });
	guideUI_[1] = std::make_unique<Sprite>();
	guideUI_[1]->Initialize("", "guide_pad_A.png");
	//guideUI_[1].reset(Sprite::Create("", "guide_pad_A.png"));
	guideUI_[1]->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 * 3 });
}

void GameOver::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		sceneNum = TITLE_SCENE;
	}
}

void GameOver::Draw() {
	// UI
	for (int i = 0; i < 2; i++) {
		guideUI_[i]->Draw();
	}
}

void GameOver::Finalize() {
	// 基底クラスの解放
	IScene::Finalize();
}