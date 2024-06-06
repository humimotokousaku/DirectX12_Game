#include "Gauss.h"

Gauss::Gauss()
{

}

void Gauss::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
}

void Gauss::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	IPostEffect::Draw(psoNum, resource);
}

void Gauss::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void Gauss::PostDrawScene() {
	IPostEffect::PostDrawScene();
}