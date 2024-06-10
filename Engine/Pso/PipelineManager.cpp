#include "PipelineManager.h"
#include "ConvertString.h"
#include <format>
#include <cassert>

PipelineManager* PipelineManager::GetInstance() {
	static PipelineManager instance;

	return &instance;
}

PipelineManager::~PipelineManager() {
	for (IPSO* postEffect : postEffect_) {
		delete postEffect;
	}
	postEffect_.clear();
}

void PipelineManager::Initialize() {
	// DXCの初期化
	DXCInitialize();

	// 3Dのオブジェクトに使用するPSO
	object3dPSO_ = Object3dPSO::GetInstance();
	object3dPSO_->Init(dxcUtils_, dxcCompiler_, includeHandler_,"Object3d.VS.hlsl","Object3d.PS.hlsl");
	object3dPSO_->CreatePSO();
	// スキニングを使用しているオブジェクトに使用する
	skinningPSO_ = SkinningPSO::GetInstance();
	skinningPSO_->Init(dxcUtils_, dxcCompiler_, includeHandler_, "SkinningObject3d.VS.hlsl", "Object3d.PS.hlsl");
	skinningPSO_->CreatePSO();
	// skyboxを使用しているオブジェクトに使用する
	skyboxPSO_ = SkyboxPSO::GetInstance();
	skyboxPSO_->Init(dxcUtils_, dxcCompiler_, includeHandler_, "Skybox.VS.hlsl", "Skybox.PS.hlsl");
	skyboxPSO_->CreatePSO();
	// 3Dの線に使用するPSO
	linePSO_ = LinePSO::GetInstance();
	linePSO_->Init(dxcUtils_, dxcCompiler_, includeHandler_,"LineVS.hlsl", "LinePS.hlsl");
	linePSO_->CreatePSO();
	// particleに使用するPSO
	particlePSO_ = ParticlePSO::GetInstance();
	particlePSO_->Init(dxcUtils_, dxcCompiler_, includeHandler_,"Particle.VS.hlsl","Particle.PS.hlsl");
	particlePSO_->CreatePSO();

#pragma region postEffectに使用するPSO
	// 何もしない
	PostEffectPSO* normal = new PostEffectPSO(dxcUtils_, dxcCompiler_, includeHandler_, "PostEffectTestVS.hlsl", "PostEffectTestPS.hlsl");
	postEffect_.push_back(normal);
	// RadialBlur
	RadialBlurPSO* radialBlur = new RadialBlurPSO(dxcUtils_, dxcCompiler_, includeHandler_, "PostEffectTestVS.hlsl", "RadialBlur.PS.hlsl");
	postEffect_.push_back(radialBlur);
	// Gauss
	GaussPSO* gauss = new GaussPSO(dxcUtils_, dxcCompiler_, includeHandler_, "PostEffectTestVS.hlsl", "GaussianFilter.PS.hlsl");
	postEffect_.push_back(gauss);
	// Dissolve
	//DissolvePSO* dissolve = new DissolvePSO(dxcUtils_, dxcCompiler_, includeHandler_, "PostEffectTestVS.hlsl", "Dissolve.PS.hlsl");
	//postEffect_.push_back(dissolve);
	// Bloom
	BloomPSO* bloom = new BloomPSO(dxcUtils_, dxcCompiler_, includeHandler_, "PostEffectTestVS.hlsl", "BloomPS.hlsl");
	postEffect_.push_back(bloom);
#pragma endregion

	// ビューポートの生成
	CreateViewport();
	// シザー矩形の生成
	CreateScissor();
}

void PipelineManager::PreDraw() {
	// ポストエフェクトの描画前処理
	DirectXCommon::GetInstance()->GetCommandList()->RSSetViewports(1, &viewport_); // Viewportを設定
	DirectXCommon::GetInstance()->GetCommandList()->RSSetScissorRects(1, &scissorRect_); // Scirssorを設定
}

void PipelineManager::DXCInitialize() {
	HRESULT hr;
	// dxCompilerの初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するために設定を行っておく
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

void PipelineManager::CreateViewport() {
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = (float)WinApp::kClientWidth_;
	viewport_.Height = (float)WinApp::kClientHeight_;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

void PipelineManager::CreateScissor() {
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = WinApp::kClientWidth_;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApp::kClientHeight_;
}
