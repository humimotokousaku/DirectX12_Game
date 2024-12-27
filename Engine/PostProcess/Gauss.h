#pragma once
#include "IPostEffect.h"
#include "PostEffectStructs.h"

class Gauss : public IPostEffect {
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

#pragma region Setter
	/// <summary>
	/// パラメータを設定
	/// </summary>
	/// <param name="radialBlurData">ガウスブラーの数値</param>
	void SetGaussData(GaussData gaussData) { *gaussData_ = gaussData; }
#pragma endregion

private:
	GaussData* gaussData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> gaussResource_;
};
