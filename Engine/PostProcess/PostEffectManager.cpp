#include "PostEffectManager.h"
#include "PostEffect.h"
#include "RadialBlur.h"
#include "Bloom.h"
#include "Gauss.h"
#include "Dissolve.h"
#include "Outline.h"

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
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->SetTranslate(Vector3{ 0,0,0 });
	// 何もしない
	PostEffect* normal = new PostEffect();
	normal->Initialize();
	renderTexture_.push_back(normal->GetRenderTexture());
	postEffect_.push_back(normal);
	// outline
	//Outline* outline = new Outline();
	//outline->Initialize();
	//renderTexture_.push_back(outline->GetRenderTexture());
	//postEffect_.push_back(outline);
	// RadialBlur
	RadialBlur* radialBlur = new RadialBlur();
	radialBlur->Initialize();
	renderTexture_.push_back(radialBlur->GetRenderTexture());
	postEffect_.push_back(radialBlur);
	// Gauss
	Gauss* gauss = new Gauss();
	gauss->Initialize();
	renderTexture_.push_back(gauss->GetRenderTexture());
	postEffect_.push_back(gauss);
	// Dissolve
	//Dissolve* dissolve = new Dissolve();
	//dissolve->Initialize();
	//renderTexture_.push_back(normal->GetRenderTexture());
	//postEffect_.push_back(dissolve);
	// Bloom
	Bloom* bloom = new Bloom();
	bloom->Initialize();
	renderTexture_.push_back(bloom->GetRenderTexture());
	postEffect_.push_back(bloom);
}

void PostEffectManager::PreDraw() {
	postEffect_[NORMAL]->PreDrawScene();
}

void PostEffectManager::PostDraw() {
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