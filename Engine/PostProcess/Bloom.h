#pragma once
#include "Sprite.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "PostEffectPSO.h"
#include "IPostEffect.h"
#include <Windows.h>

class Bloom : public IPostEffect {
private:// 構造体
	struct BloomData {
		bool isActive;	// 使用するか
		float strength;	// ぼかしの強さ
	};
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Bloom();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Bloom()override = default;

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

private:// プライベートなメンバ変数
	BloomData* bloomData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> bloomResource_;
};