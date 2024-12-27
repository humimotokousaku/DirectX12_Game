#include "Dissolve.h"

Dissolve::Dissolve() {

}

void Dissolve::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();

	// ブラーの情報を書き込む
	dissolveResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(DissolveData)).Get();
	// 書き込むためのアドレスを取得
	dissolveResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&dissolveData_));
}

void Dissolve::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	IPostEffect::Draw(psoNum, dissolveResource_);
}

void Dissolve::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void Dissolve::PostDrawScene() {
	IPostEffect::PostDrawScene();
}