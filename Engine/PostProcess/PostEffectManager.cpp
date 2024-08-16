#include "PostEffectManager.h"

PostEffectManager* PostEffectManager::GetInstance() {
	static PostEffectManager instance;
	return &instance;
}

PostEffectManager::~PostEffectManager() {
	for (IPostEffect* postEffect : postEffect_) {
		delete postEffect;
	}
	postEffect_.clear();
}

void PostEffectManager::Initialize() {
	// 何もしない
	normal_ = new PostEffect();
	normal_->Initialize();
	renderTexture_.push_back(normal_->GetRenderTexture());
	postEffect_.push_back(normal_);
	// RadialBlur
	radialBlur_ = new RadialBlur();
	radialBlur_->Initialize();
	renderTexture_.push_back(radialBlur_->GetRenderTexture());
	radialBlurData_.isActive = true;
	radialBlurData_.blurWidth = 0.005f;
	radialBlurData_.center = { 0.5f, 0.5f };
	radialBlur_->SetRadialBlurData(radialBlurData_);
	postEffect_.push_back(radialBlur_);
	// Gauss
	gauss_ = new Gauss();
	gauss_->Initialize();
	renderTexture_.push_back(gauss_->GetRenderTexture());
	gaussData_.isActive = false;
	gaussData_.strength = 1.0f;
	gauss_->SetGaussData(gaussData_);
	postEffect_.push_back(gauss_);
	// Bloom
	bloom_ = new Bloom();
	bloom_->Initialize();
	renderTexture_.push_back(bloom_->GetRenderTexture());
	bloomData_.isActive = true;
	bloomData_.strength = 1.0f;
	bloomData_.threshold = 0.3f;
	bloom_->SetBloomData(bloomData_);
	postEffect_.push_back(bloom_);
}

void PostEffectManager::PreDraw() {
	postEffect_[NORMAL]->PreDrawScene();

}

void PostEffectManager::PostDraw() {
	radialBlur_->SetRadialBlurData(radialBlurData_);
	gauss_->SetGaussData(gaussData_);
	bloom_->SetBloomData(bloomData_);

	postEffect_[NORMAL]->PostDrawScene();
	for (int i = 0; i < COUNT - 1; i++) {
		postEffect_[i + 1]->PreDrawScene();
		postEffect_[i]->Draw(i);
		postEffect_[i + 1]->PostDrawScene();
	}
}

void PostEffectManager::Draw() {
	postEffect_[COUNT - 1]->Draw(COUNT - 1);

	// ポストエフェクトを使用しないスプライト
	for (Sprite* sprite : sprites_) {
		sprite->Draw();
	}
}