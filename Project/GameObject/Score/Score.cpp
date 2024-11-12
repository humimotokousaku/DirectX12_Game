#include "Score.h"
#include "Lerp.h"

Score* Score::GetInstance() {
	static Score instance;
	return &instance;
}

void Score::Initialize() {
	textureManager_ = TextureManager::GetInstance();
	postEffectManager_ = PostEffectManager::GetInstance();

	for (int i = 0; i < numbersTexture_.size(); i++) {
		textureManager_->LoadTexture("Textures/UI", std::to_string(i) + ".png");
		numbersTexture_[i] = textureManager_->GetSrvIndex("Textures/UI", std::to_string(i) + ".png");
	}
	for (int i = 0; i < 7; i++) {
		numSprite_[i] = std::make_unique<Sprite>();
		numSprite_[i]->Initialize("Textures/UI", "0.png");
		numSprite_[i]->SetAnchorPoint(Vector2{ 0.5f, 0.5f });
		numSprite_[i]->SetPos(Vector2{ 1120.0f + 16.0f * i, 192.0f });
		postEffectManager_->AddSpriteList(numSprite_[i].get());

		eachScore_[i] = 0;
	}
	goalScore_ = 0;
	score_ = 0;
	tempScore_ = 0;
}

void Score::Update() {
	// スコアの挙動確認用
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		score_ += 11;
	}
#endif // _DEBUG

	scoreAnim_.Update();

	for (int i = 0; i < 7; i++) {
		if (i == 0) {
			tempScore_ = score_;
			eachScore_[i] = tempScore_ / 1000000;
			tempScore_ = tempScore_ % 10000000;
		}
		if (i == 1) {
			eachScore_[i] = tempScore_ / 100000;
			tempScore_ = tempScore_ % 100000;
		}
		if (i == 2) {
			eachScore_[i] = tempScore_ / 10000;
			tempScore_ = tempScore_ % 10000;
		}
		if (i == 3) {
			eachScore_[i] = tempScore_ / 1000;
			tempScore_ = tempScore_ % 1000;
		}
		if (i == 4) {
			eachScore_[i] = tempScore_ / 100;
			tempScore_ = tempScore_ % 100;
		}
		if (i == 5) {
			eachScore_[i] = tempScore_ / 10;
			tempScore_ = tempScore_ % 10;
		}
		if (i == 6) {
			eachScore_[i] = tempScore_ / 10;
		}
		// それぞれの桁に適した数値の画像を差し替える
		numSprite_[i]->SetTextureIndex(numbersTexture_[eachScore_[i]]);
	}
}

void Score::AddScore(int value) {
	int preScore = score_;
	goalScore_ += value;
	scoreAnim_.SetFirstAnimData(&score_, preScore, goalScore_, 20, Easings::EaseOutCubic);
	scoreAnim_.SetIsStart(true);
}
