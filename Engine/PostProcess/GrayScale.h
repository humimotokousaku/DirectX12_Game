#pragma once
#include "IPostEffect.h"

class GrayScale : public IPostEffect {
private:// 構造体
	struct GrayScaleData {
		bool isActive;
		float strength;
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GrayScale();

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
	GrayScaleData* grayData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> grayResource_;
};

