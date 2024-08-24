#include "SceneTransition.h"

SceneTransition* SceneTransition::GetInstance() {
	static SceneTransition instance;
	return &instance;
}

void SceneTransition::Initialize() {
	sprite_.Initialize("DefaultTexture", "white.png");
	sprite_.SetSize(Vector2{ 1280.0f, 720.0f });
	sprite_.worldTransform_.translate.x = WinApp::kClientWidth_ / 2;
	sprite_.worldTransform_.translate.y = WinApp::kClientHeight_ / 2;
	PostEffectManager::GetInstance()->AddSpriteList(&sprite_);
	sprite_.SetColor(Vector4{ 0,0,0,0 });

	beginAnim_.SetAnimData(sprite_.GetColorP(), Vector4{ 0,0,0,0 }, Vector4{ 0,0,0,1 }, 0, 30, "sceneTransition_00", Easings::EaseInOutExpo);
	endAnim_.SetAnimData(sprite_.GetColorP(), Vector4{ 0,0,0,1 }, Vector4{ 0,0,0,0 }, 0, 30, "sceneTransition_00", Easings::EaseInOutExpo);

	isStart_ = false;
}

void SceneTransition::Update() {
	// 初期化
	if (behaviorRequest_) {
		//  振るまいを変更
		behavior_ = behaviorRequest_.value();
		switch (behavior_) {
		case Behavior::None:
		default:
			B_NoneInit();
			break;
		case Behavior::FadeIn:
			B_FadeInInit();
			break;
		case Behavior::FadeOut:
			B_FadeOutInit();
			break;
		case Behavior::End:
			B_EndInit();
			break;
		}
		// 振るまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}
	// 更新処理
	switch (behavior_) {
	case Behavior::None:
	default:
		B_NoneUpdate();
		break;
	case Behavior::FadeIn:
		beginAnim_.Update();
		B_FadeInUpdate();
		break;
	case Behavior::FadeOut:
		endAnim_.Update();
		B_FadeOutUpdate();
		break;
	case Behavior::End:
		B_EndUpdate();
		break;
	}
}

#pragma region Behavior
void SceneTransition::B_NoneInit() {
	beginAnim_.ResetData();
	endAnim_.ResetData();
	isStart_ = false;
}
void SceneTransition::B_NoneUpdate() {
	if (isStart_) {
		behaviorRequest_ = Behavior::FadeIn;
	}
}

void SceneTransition::B_FadeInInit() {
	beginAnim_.SetIsStart(true);
}
void SceneTransition::B_FadeInUpdate() {
	if (beginAnim_.GetIsEnd()) {
		behaviorRequest_ = Behavior::FadeOut;
		sceneTransitionSignal_ = true;
	}
}

void SceneTransition::B_FadeOutInit() {
	endAnim_.SetIsStart(true);
}
void SceneTransition::B_FadeOutUpdate() {
	if (endAnim_.GetIsEnd()) {
		behaviorRequest_ = Behavior::End;
	}
}

void SceneTransition::B_EndInit() {
	beginAnim_.ResetData();
	endAnim_.ResetData();
	isStart_ = false;
}
void SceneTransition::B_EndUpdate() {
	behaviorRequest_ = Behavior::None;
}
#pragma endregion