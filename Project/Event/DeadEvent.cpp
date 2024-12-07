#include "DeadEvent.h"

void DeadEvent::Initialize(Player* player, FollowCamera* followCamera, RailCamera* railCamera) {
	// インスタンスを取得
	postEffectManager_ = PostEffectManager::GetInstance();

	player_ = player;
	followCamera_ = followCamera;
	railCamera_ = railCamera;

	// クリア時のUI
	gameOver_ = std::make_unique<Sprite>();
	gameOver_->Initialize("Textures/UI", "gameOver.png");
	gameOver_->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,-100.0f });
	gameOver_->isActive_ = false;
	postEffectManager_->AddSpriteList(gameOver_.get());

	// クリア時のUIアニメーション
	gameOverUIAnim_.SetAnimData(&gameOver_->worldTransform_.translate, Vector3{ (float)WinApp::kClientWidth_ / 2,-100.0f,0.0f }, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, 30, Easings::EaseInQuart);
	gameOverUIAnim_.SetAnimData(&gameOver_->worldTransform_.translate, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, 60, Easings::EaseInQuart);
	gameOverUIAnim_.SetAnimData(&gameOver_->worldTransform_.translate, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, Vector3{ (float)WinApp::kClientWidth_ / 2,-100.0f,0.0f }, 30, Easings::EaseInQuart);
}

void DeadEvent::Update() {
	if (!isActive_) { return; }

	// 死亡アニメーション
	player_->DeadEffect(isEnd_);
	// 演出がすべて終了したら演出中フラグを切る
	if (isEnd_) { isActive_ = false; }

	// レールカメラの更新
	railCamera_->Update();
	// 追従カメラを更新
	followCamera_->Update();

	// ゲームオーバーUIのアニメーション更新
	gameOverUIAnim_.Update();
}
