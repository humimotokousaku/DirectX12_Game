#include "GaugeUI.h"
#include "ImGuiManager.h"
#include <cassert>

void GaugeUI::Initialize() {
	textureManager_ = TextureManager::GetInstance();
	psoManager_ = PipelineManager::GetInstance();

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Textures/UI", "guide_BulletGauge.png");
	sprite_->worldTransform_.translate = { (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2,1 };
}

void GaugeUI::Draw() {
	// 描画しないなら早期リターン
	if (!isActive_) { return; }

	sprite_->Draw();

	sprite_->ImGuiAdjustParameter();
}