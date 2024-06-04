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
	// Bloom
	Bloom* bloom = new Bloom();
	bloom->Initialize();
	postEffect_.push_back(bloom);
#pragma region シェーダ内のパラメータを調整するための準備
	//// ブラーの情報を書き込む
	//bloomResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(BloomData)).Get();
	//// 書き込むためのアドレスを取得
	//bloomResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&bloomData_));
#pragma endregion
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