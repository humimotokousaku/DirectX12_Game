#include "PostEffect.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "WinApp.h" 
#include "SrvManager.h"

PostEffect::PostEffect()
{

}

void PostEffect::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
}

void PostEffect::Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
	IPostEffect::Draw(psoNum, resource);
}

void PostEffect::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void PostEffect::PostDrawScene() {
	IPostEffect::PostDrawScene();
}