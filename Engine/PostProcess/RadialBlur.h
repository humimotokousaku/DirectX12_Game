#pragma once
#include "Sprite.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "PostEffectPSO.h"
#include "IPostEffect.h"

class RadialBlur : public IPostEffect {
private:// 構造体
	// ブラー
	struct RadialBlurData {
		bool isActive;	 // 使用するか
		float blurWidth; // ブラーの強さ
		Vector2 center;
	};
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RadialBlur();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~RadialBlur()override = default;

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
	void Draw(uint32_t psoNum, Microsoft::WRL::ComPtr<ID3D12Resource> resource)override;

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDrawScene()override;

	/// <summary>
	/// 描画後の処理
	/// </summary>
	void PostDrawScene()override;

private:
	RadialBlurData* radialBlurData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> radialBlurResource_;
};