#include "Outline.h"

Outline::Outline()
{

}

void Outline::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
	isActive_ = true;
}

void Outline::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	IPostEffect::Draw(psoNum, resource);
}

void Outline::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void Outline::PostDrawScene() {
	IPostEffect::PostDrawScene();
}