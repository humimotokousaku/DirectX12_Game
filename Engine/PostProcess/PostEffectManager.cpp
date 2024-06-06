#include "PostEffectManager.h"

PostEffectManager::~PostEffectManager() {
	for (IPostEffect* postEffect : postEffect_) {
		delete postEffect;
	}
	postEffect_.clear();
}

void PostEffectManager::Initialize() {
	// 何もしない
	PostEffect* normal = new PostEffect();
	normal->Initialize();
	postEffect_.push_back(normal);
	// RadialBlur
	RadialBlur* radialBlur = new RadialBlur();
	radialBlur->Initialize();
	postEffect_.push_back(radialBlur);
	// Gauss
	Gauss* gauss = new Gauss();
	gauss->Initialize();
	postEffect_.push_back(gauss);
	// Dissolve
	//Dissolve* dissolve = new Dissolve();
	//dissolve->Initialize();
	//postEffect_.push_back(dissolve);
	// Bloom
	Bloom* bloom = new Bloom();
	bloom->Initialize();
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
}