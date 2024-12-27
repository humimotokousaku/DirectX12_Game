#pragma once
#include "Sprite.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "PostEffectPSO.h"
#include "IPostEffect.h"

class Vigneting : public IPostEffect {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Vigneting();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Vigneting()override = default;

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

#pragma region Setter
	/// <summary>
	/// パラメータを設定
	/// </summary>
	/// <param name="vignetingData">ヴィネットの情報</param>
	void SetVignetingData(VignetingData vignetingData) { *vignetingData_ = vignetingData; }
#pragma endregion

private:
	VignetingData* vignetingData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> vignetingDataResource_;
};

