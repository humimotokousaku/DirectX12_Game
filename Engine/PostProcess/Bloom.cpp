#include "Bloom.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "WinApp.h" 
#include "SrvManager.h"

Bloom::Bloom() {

}

void Bloom::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
#pragma region シェーダ内のパラメータを調整するための準備
	// ブラーの情報を書き込む
	bloomResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(BloomData)).Get();
	// 書き込むためのアドレスを取得
	bloomResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&bloomData_));
#pragma endregion
}

void Bloom::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	IPostEffect::Draw(psoNum, bloomResource_);
}

void Bloom::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void Bloom::PostDrawScene() {
	IPostEffect::PostDrawScene();
}