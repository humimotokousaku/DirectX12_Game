#pragma once
#include "IPSO.h"

class Object3dPSO : public IPSO {
public:// メンバ関数
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

#pragma region Setter
	/// <summary>
	/// 描画前に積むコマンド
	/// </summary>
	void SetCommand(Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr) override{
		// シグネチャの設定
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(psoData_.rootSignature_.Get());
		// PSOを設定
		dxCommon_->GetCommandList()->SetPipelineState(psoData_.graphicsPipelineState_.Get());
		// 形状を設定
		dxCommon_->GetCommandList()->IASetPrimitiveTopology(psoData_.primitiveTopology_);
		// 環境マップ用のテクスチャ
		SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(8, ddsTexture_);
	}
#pragma endregion

private:// プライベートなメンバ変数
	uint32_t ddsTexture_;
};