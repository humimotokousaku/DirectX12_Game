#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"
#include "ImGuiManager.h"
#include "TextureManager.h"

#include "Object3dPSO.h"
#include "SkinningPSO.h"
#include "LinePSO.h"
#include "ParticlePSO.h"

#include "DissolvePSO.h"
#include "GaussPSO.h"
#include "SmoothingPSO.h"
#include "RadialBlurPSO.h"
#include "GaussPSO.h"
#include "BloomPSO.h"
#include "PostEffectPSO.h"

#include <dxcapi.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

enum BlendMode {
	// ブレンドなし
	kBlendModeNone,
	// 通常αブレンド
	kBlendModeNormal,
	// 加算
	kBlendModeAdd,
	// 減算
	kBlendModeSubtract,
	// 乗算
	kBlendModeMultiply,
	// スクリーン
	kBlendModeScreen,
	// 利用してはいけない
	kCountOfBlendMode,
};

/// <summary>
/// 全てのPSOインスタンスを作成するクラス
/// </summary>
class PipelineManager {
public:
	/// <summary>
	/// シングルトン
	/// </summary>
	static PipelineManager* GetInstance();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PipelineManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前の処理
	/// </summary>
	void PreDraw();
	//void PostDraw();

	/// Setter

	// ポストエフェクトで使用するコマンドをセット
	void SetPostEffectPSO(int index, Microsoft::WRL::ComPtr<ID3D12Resource> resource) { postEffect_[index]->SetCommand(resource); }
	void SetObject3dPSO() { object3dPSO_->SetCommand(); }
	void SetSkinningPSO() { skinningPSO_->SetCommand(); }
	void SetLinePSO() { linePSO_->SetCommand(); }
	void SetParticlePSO() { particlePSO_->SetCommand(); }

private:// プライベートな関数
	void DXCInitialize();
	// ビューポート
	void CreateViewport();
	// シザー矩形
	void CreateScissor();

private:
	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;

	// PSO
	Object3dPSO* object3dPSO_;
	SkinningPSO* skinningPSO_;
	LinePSO* linePSO_;
	ParticlePSO* particlePSO_;
	// ポストエフェクトで使用するPSO
	std::vector<IPSO*> postEffect_;

	std::vector<const char*> postEffectPSName;
};
