#pragma once
#include "IPostEffect.h"

class Dissolve : public IPostEffect {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Dissolve();

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
	/// <param name="vignetingData">Dissolveの情報</param>
	void SetDissolveData(DissolveData dissolveData) { *dissolveData_ = dissolveData; }
#pragma endregion

private:
	DissolveData* dissolveData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource_;
};

