#include "SceneTransition.h"

SceneTransition* SceneTransition::GetInstance() {
	static SceneTransition instance;
	return &instance;
}

void SceneTransition::Initialize() {
	for (int i = 0; i < sprite_.size(); i++) {
		for (int j = 0; j < sprite_[0].size(); j++) {
			sprite_[i][j].Initialize("Textures/DefaultTexture", "white.png");
			sprite_[i][j].SetSize(Vector2{ 0.0f, 0.0f });
			sprite_[i][j].worldTransform_.translate.x = 40.0f + j * 80.0f;
			sprite_[i][j].worldTransform_.translate.y = 40.0f + i * 80.0f;
			sprite_[i][j].isActive_ = false;
			PostEffectManager::GetInstance()->AddSpriteList(&sprite_[i][j]);
			sprite_[i][j].SetColor(Vector4{ 0,0,0,1 });
		}
	}
	for (int i = 0; i < sprite_.size(); i++) {
		for (int j = 0; j < sprite_[0].size(); j++) {
			beginAnim_[i][j].SetAnimData(sprite_[i][j].GetSizeP(), Vector2{ 0,0 }, Vector2{ 80,80 }, 40, Easings::EaseOutExpo);
			endAnim_[i][j].SetAnimData(sprite_[i][j].GetSizeP(), Vector2{ 80,80 }, Vector2{ 0,0 }, 40, Easings::EaseOutExpo);
		}
	}
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
		for (int i = 0; i < beginAnim_.size(); i++) {
			for (int j = 0; j < beginAnim_[0].size(); j++) {
				beginAnim_[i][j].Update();
			}
		}
		B_FadeInUpdate();
		break;
	case Behavior::FadeOut:
		for (int i = 0; i < endAnim_.size(); i++) {
			for (int j = 0; j < endAnim_[0].size(); j++) {
				endAnim_[i][j].Update();
			}
		}
		B_FadeOutUpdate();
		break;
	case Behavior::End:
		B_EndUpdate();
		break;
	}
}

#pragma region Behavior
bool SceneTransition::CheckBlock(Vector2 pos) {
	Vector2 a = {
		WinApp::kClientWidth_ / 2.0f - pos.x,
		WinApp::kClientHeight_ / 2.0f - pos.y
	};
	if (radius_ >= Length(a)) {
		return true;
	}
	return false;
}
void SceneTransition::B_NoneInit() {
	for (int i = 0; i < beginAnim_.size(); i++) {
		for (int j = 0; j < beginAnim_[0].size(); j++) {
			beginAnim_[i][j].ResetData();
		}
	}
	for (int i = 0; i < endAnim_.size(); i++) {
		for (int j = 0; j < endAnim_[0].size(); j++) {
			endAnim_[i][j].ResetData();
		}
	}
	isStart_ = false;
	radius_ = 0.0f;
}
void SceneTransition::B_NoneUpdate() {
	if (isStart_) {
		behaviorRequest_ = Behavior::FadeIn;
	}
}

void SceneTransition::B_FadeInInit() {
	for (int i = 0; i < sprite_.size(); i++) {
		for (int j = 0; j < sprite_[0].size(); j++) {
			sprite_[i][j].isActive_ = true;
		}
	}
	radius_ = 0.0f;
}
void SceneTransition::B_FadeInUpdate() {
	for (int i = 0; i < beginAnim_.size(); i++) {
		for (int j = 0; j < beginAnim_[0].size(); j++) {
			if (CheckBlock(sprite_[i][j].GetPos())) {
				beginAnim_[i][j].SetIsStart(true);
			}
		}
	}

	if (beginAnim_[8][15].GetIsEnd()) {
		behaviorRequest_ = Behavior::FadeOut;
		sceneTransitionSignal_ = true;
	}

	radius_ += 20;
}

void SceneTransition::B_FadeOutInit() {
	radius_ = 0.0f;
}
void SceneTransition::B_FadeOutUpdate() {
	for (int i = 0; i < endAnim_.size(); i++) {
		for (int j = 0; j < endAnim_[0].size(); j++) {
			if (CheckBlock(sprite_[i][j].GetPos())) {
				endAnim_[i][j].SetIsStart(true);
			}
		}
	}

	if (endAnim_[8][15].GetIsEnd()) {
		behaviorRequest_ = Behavior::End;
	}

	radius_ += 20;
}

void SceneTransition::B_EndInit() {
	for (int i = 0; i < sprite_.size(); i++) {
		for (int j = 0; j < sprite_[0].size(); j++) {
			sprite_[i][j].isActive_ = false;
		}
	}
	for (int i = 0; i < beginAnim_.size(); i++) {
		for (int j = 0; j < beginAnim_[0].size(); j++) {
			beginAnim_[i][j].ResetData();
		}
	}
	for (int i = 0; i < endAnim_.size(); i++) {
		for (int j = 0; j < endAnim_[0].size(); j++) {
			endAnim_[i][j].ResetData();
		}
	}
	isStart_ = false;
	radius_ = 0.0f;
}
void SceneTransition::B_EndUpdate() {
	behaviorRequest_ = Behavior::None;
}
#pragma endregion