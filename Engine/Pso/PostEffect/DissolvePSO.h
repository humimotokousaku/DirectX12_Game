#pragma once
#include "IPSO.h"

class DissolvePSO : public IPSO
{
public:
	///
	/// Default Method
	///

	DissolvePSO(IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, const std::string& VS_fileName, const std::string& PS_fileName);
	~DissolvePSO() = default;

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

	/// <summary>
	/// 描画前に積むコマンド
	/// </summary>
	void SetCommand(Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr) {
		// シグネチャの設定
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(psoData_.rootSignature_.Get());
		// PSOを設定
		dxCommon_->GetCommandList()->SetPipelineState(psoData_.graphicsPipelineState_.Get());
		// 形状を設定
		dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Dissolve用のテクスチャ
		SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(4, dissolveTextureHandle_);
		// DissolveData
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, resource.Get()->GetGPUVirtualAddress());
	}

private:// プライベートな変数
	// dissolve用のテクスチャ
	uint32_t dissolveTextureHandle_;
};