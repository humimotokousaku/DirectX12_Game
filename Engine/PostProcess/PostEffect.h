#pragma once
#include "Sprite.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "PostEffectPSO.h"
#include "IPostEffect.h"
#include <Windows.h>

class PostEffect : public IPostEffect {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PostEffect();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	//void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr)override;

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDrawScene()override;

	/// <summary>
	/// 描画後の処理
	/// </summary>
	void PostDrawScene()override;

private:

};