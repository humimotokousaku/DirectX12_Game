#include "Dissolve.h"

Dissolve::Dissolve() {

}

void Dissolve::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
}

void Dissolve::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	IPostEffect::Draw(psoNum);
}

void Dissolve::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void Dissolve::PostDrawScene() {
	IPostEffect::PostDrawScene();
}