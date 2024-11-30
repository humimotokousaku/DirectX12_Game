#include "Vigneting.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "WinApp.h" 
#include "SrvManager.h"

Vigneting::Vigneting() {

}

void Vigneting::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
#pragma region シェーダ内のパラメータを調整するための準備
	// ブラーの情報を書き込む
	vignetingDataResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VignetingData)).Get();
	// 書き込むためのアドレスを取得
	vignetingDataResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vignetingData_));
#pragma endregion
}

void Vigneting::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	IPostEffect::Draw(psoNum, vignetingDataResource_);
}

void Vigneting::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void Vigneting::PostDrawScene() {
	IPostEffect::PostDrawScene();
}