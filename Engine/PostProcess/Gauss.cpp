#include "Gauss.h"

Gauss::Gauss() {

}

void Gauss::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
#pragma region シェーダ内のパラメータを調整するための準備
	// ブラーの情報を書き込む
	gaussResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(GaussData)).Get();
	// 書き込むためのアドレスを取得
	gaussResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&gaussData_));
#pragma endregion
}

void Gauss::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	IPostEffect::Draw(psoNum, gaussResource_);
}

void Gauss::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void Gauss::PostDrawScene() {
	IPostEffect::PostDrawScene();
}