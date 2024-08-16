#include "RadialBlur.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "WinApp.h" 
#include "SrvManager.h"

RadialBlur::RadialBlur() {

}

void RadialBlur::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
#pragma region シェーダ内のパラメータを調整するための準備
	// ブラーの情報を書き込む
	radialBlurResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(RadialBlurData)).Get();
	// 書き込むためのアドレスを取得
	radialBlurResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&radialBlurData_));
#pragma endregion
}

void RadialBlur::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	IPostEffect::Draw(psoNum, radialBlurResource_);
}

void RadialBlur::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void RadialBlur::PostDrawScene() {
	IPostEffect::PostDrawScene();
}