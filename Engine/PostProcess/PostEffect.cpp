#include "PostEffect.h"

PostEffect::PostEffect()
{

}

void PostEffect::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
	isActive_ = true;
}

void PostEffect::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	PipelineManager::GetInstance()->SetPostEffectPSO(psoNum, resource);
	IPostEffect::Draw(psoNum, resource);
}

void PostEffect::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void PostEffect::PostDrawScene() {
	IPostEffect::PostDrawScene();
}