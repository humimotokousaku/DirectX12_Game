#include "HitSystem.h"
#include "PostEffectManager.h"

void HitSystem::Initialize() {
	// インスタンスを取得
	input_ = Input::GetInstance();

	// 被弾時のフィルター
	hitSprite_ = std::make_unique<Sprite>();
	hitSprite_->Initialize("Textures/DefaultTexture", "white.png");
	hitSprite_->SetSize(Vector2{ (float)WinApp::kClientWidth_,(float)WinApp::kClientHeight_ });
	hitSprite_->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2.0f,(float)WinApp::kClientHeight_ / 2.0f });
	hitSprite_->SetColor(Vector4{ 1.0f, 0.0f, 0.0f, 0.5f });
	hitSprite_->isActive_ = false;
	PostEffectManager::GetInstance()->AddSpriteList(hitSprite_.get());

	// 被弾時の自機アニメーション
	hitAnim_.SetAnimData(&hitRotate_.z, 0.0f, (float)std::numbers::pi / 3.0f, 6, Easings::EaseOutExpo);
	hitAnim_.SetAnimData(&hitRotate_.z, (float)std::numbers::pi / 3.0f, 0.0f, 6, Easings::EaseOutBounce);

	// 被弾時のフィルターアニメーション
	hitSpriteAnim_.SetAnimData(hitSprite_->GetColorP(), hitSprite_->GetColor(), Vector4{ 1.0f, 0.0f, 0.0f, 0.0f }, 30, Easings::EaseOutExpo);

	isHit_ = false;
}

void HitSystem::Update() {
	// 被弾演出の更新処理
	HitUpdate();

	// 当たった瞬間にコントローラーを振動させる
	if (isHit_ && !isPreHit_) {
		input_->GamePadVibration(0, 65535, 65535);
	}
	isPreHit_ = isHit_;

	// 演出開始してい無ければ終了
	if (!isHit_) { return; }

	// 被弾時の自機アニメーション
	hitAnim_.SetIsStart(true);
	// 被弾時のフィルターアニメーション
	hitSpriteAnim_.SetIsStart(true);
	// 被弾時のフィルター表示
	hitSprite_->isActive_ = true;

	currentFrame_--;
}

void HitSystem::Draw() {
	hitSprite_->Draw();
}

void HitSystem::HitUpdate() {
	// 被弾時の自機のアニメーション
	hitAnim_.Update();
	// 被弾時のフィルターアニメーション
	hitSpriteAnim_.Update();

	// 画面の揺れを行う
	if (currentFrame_ <= 0) {
		currentFrame_ = endFrame_;
		isHit_ = false;
		hitSprite_->isActive_ = false;
		input_->GamePadVibration(0, 0, 0);
	}
}
