#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"
#include "ImGuiManager.h"
#include "TextureManager.h"

#include "Object3dPSO.h"
#include "SkinningPSO.h"
#include "SkyboxPSO.h"
#include "LinePSO.h"
#include "ParticlePSO.h"
#include "OutlinePSO.h"
#include "DissolvePSO.h"
#include "GaussPSO.h"
#include "SmoothingPSO.h"
#include "RadialBlurPSO.h"
#include "BloomPSO.h"
#include "PostEffectPSO.h"
#include "VignetingPSO.h"
#include <array>
#include <dxcapi.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")



/// <summary>
/// 使用できるポストエフェクトの種類
/// </summary>
enum PostEffectType {
	NORMAL,
	//OUTLINE,
	RADIALBLUR,
	//SMOOTHING,
	GAUSSIAN,
	/*HIGHINTENSITY,
	BLUR,*/
	//DISSOLVE,
	BLOOM,
	COUNT
};

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
/// オブジェクトの表示モード
/// </summary>
enum FillMode {
	kFillModeSolid,		// 塗りつぶし
	kFillModeWireFrame	// ワイヤーフレーム
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

#pragma region Setter
	// ポストエフェクトで使用するコマンドをセット
	void SetPostEffectPSO(int index, Microsoft::WRL::ComPtr<ID3D12Resource> resource) { postEffect_[index]->SetCommand(resource); }
	void SetObject3dPSO(int fillMode) { object3dPSO_[fillMode]->SetCommand(); }
	void SetSkinningPSO(int fillMode) { skinningPSO_[fillMode]->SetCommand(); }
	void SetSkyboxPSO() { skyboxPSO_->SetCommand(); }
	void SetLinePSO() { linePSO_->SetCommand(); }
	void SetParticlePSO() { particlePSO_->SetCommand(); }
#pragma endregion

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
	std::array<std::unique_ptr<Object3dPSO>, 2> object3dPSO_;
	std::array<std::unique_ptr<SkinningPSO>,2> skinningPSO_;
	std::unique_ptr<SkyboxPSO> skyboxPSO_;
	std::unique_ptr<LinePSO> linePSO_;
	std::unique_ptr<ParticlePSO> particlePSO_;
	// ポストエフェクトで使用するPSO
	std::vector<IPSO*> postEffect_;

	std::vector<const char*> postEffectPSName;
};
