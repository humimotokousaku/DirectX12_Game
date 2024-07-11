#include "GrayScale.h"

GrayScale::GrayScale() {

}

void GrayScale::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
#pragma region シェーダ内のパラメータを調整するための準備
	// ブラーの情報を書き込む
	grayResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(GrayScaleData)).Get();
	// 書き込むためのアドレスを取得
	grayResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&grayData_));
#pragma endregion

	grayData_->isActive = false;
	isActive_ = grayData_->isActive;
}

void GrayScale::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
#ifdef _DEBUG
	ImGui::Begin("Gray");
	ImGui::Checkbox("isActive", &grayData_->isActive);
	ImGui::End();
#endif

	isActive_ = grayData_->isActive;
	PipelineManager::GetInstance()->SetPostEffectPSO(psoNum, grayResource_);
	IPostEffect::Draw(psoNum);
}

void GrayScale::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void GrayScale::PostDrawScene() {
	IPostEffect::PostDrawScene();
}