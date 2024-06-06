#pragma once
#include "IPostEffect.h"

class Gauss : public IPostEffect {
private:// 構造体
	struct GaussData {
		bool isActive;
		float strength;
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Gauss();

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
	GaussData* gaussData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> gaussResource_;
};
