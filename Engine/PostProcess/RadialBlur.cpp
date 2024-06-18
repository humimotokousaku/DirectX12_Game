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

	radialBlurData_->isActive = false;
	radialBlurData_->blurWidth = 0.005f;
	radialBlurData_->center = { 0.5f, 0.5f };
	isActive_ = radialBlurData_->isActive;
}

void RadialBlur::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
#ifdef _DEBUG
	ImGui::Begin("RadialBlur");
	ImGui::DragFloat("blurWidth", &radialBlurData_->blurWidth, 0.001f, 0, 100);
	ImGui::DragFloat2("center", &radialBlurData_->center.x, 0.01f, 0, 100);
	ImGui::Checkbox("isActive", &radialBlurData_->isActive);
	ImGui::End();
#endif // _DEBUG

	isActive_ = radialBlurData_->isActive;
	IPostEffect::Draw(psoNum, radialBlurResource_);
}

void RadialBlur::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void RadialBlur::PostDrawScene() {
	IPostEffect::PostDrawScene();
}