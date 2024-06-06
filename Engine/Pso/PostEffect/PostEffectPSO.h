#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "IPSO.h"
#include <dxcapi.h>

/// <summary>
/// 使用できるポストエフェクトの種類
/// </summary>
enum PostEffectType {
	NORMAL,
	RADIALBLUR,
	//SMOOTHING,
	//GAUSSIAN,
	/*HIGHINTENSITY,
	BLUR,*/
	//DISSOLVE,
	BLOOM,
	COUNT
};

/// <summary>
/// ポストエフェクトのPSO
/// </summary>
class PostEffectPSO : public IPSO
{
public:
	///
	/// Default Method
	///

	PostEffectPSO(IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, const std::string& VS_fileName, const std::string& PS_fileName);
	~PostEffectPSO() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, const std::string& VS_fileName, const std::string& PS_fileName) override;

	/// <summary>
	/// rootSignatureの作成
	/// </summary>
	void CreateRootSignature() override;

	/// <summary>
	/// PSOの作成
	/// </summary>
	void CreatePSO() override;

	///** Getter **///

	///** Setter **///

	/// <summary>
	/// 描画前に積むコマンド
	/// </summary>
	void SetCommand(Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr) {
		// シグネチャの設定
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		// PSOを設定
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		// 形状を設定
		dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

private:// プライベートな変数

};