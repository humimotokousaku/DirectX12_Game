#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include <dxcapi.h>

/// <summary>
/// PSOの基底クラス(PSOManager内で作成することを想定)
/// </summary>
class IPSO {
public:// 構造体
	struct PSOData {
		// シェーダ読み込みに使用する
		IDxcUtils* dxcUtils_;
		IDxcCompiler3* dxcCompiler_;
		IDxcIncludeHandler* includeHandler_;
		ID3DBlob* errorBlob_;
		ID3DBlob* signatureBlob_;
		IDxcBlob* vertexShaderBlob_;
		IDxcBlob* pixelShaderBlob_;
		// VSのファイル名
		std::wstring VSFilePath_;
		// PSのファイル名
		std::wstring PSFilePath_;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;
		D3D12_RASTERIZER_DESC rasterizerDesc_;
		D3D12_BLEND_DESC blendDesc_;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDescs_;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

		std::vector<D3D12_ROOT_PARAMETER> rootParameters_;
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;
		std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRange_;
		std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;

		// 形状設定
		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IPSO() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="VS_fileName">VSのファイル名</param>
	/// <param name="PS_fileName">PSのファイル名</param>
	virtual void Init(IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, const std::string& VS_fileName, const std::string& PS_fileName);

	/// <summary>
	/// rootSignatureの作成
	/// </summary>
	virtual void CreateRootSignature() = 0;

	/// <summary>
	/// PSOの作成
	/// </summary>
	virtual void CreatePSO() = 0;

#pragma region 詳細な設定の作成
	/// <summary>
	/// DescriptorRangeの作成
	/// </summary>
	/// <param name="descriptorRange"></param>
	void CreateDescriptorRange(std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRange) { psoData_.descriptorRange_ = descriptorRange; }
	/// <summary>
	/// ルートパラメータの作成
	/// </summary>
	/// <param name="rootParameters"></param>
	void CreateRootParameter(std::vector<D3D12_ROOT_PARAMETER> rootParameters) {
		psoData_.rootParameters_ = rootParameters;
		// rootParameterの設定を入れる
		psoData_.descriptionRootSignature_.pParameters = psoData_.rootParameters_.data();
		psoData_.descriptionRootSignature_.NumParameters = static_cast<UINT>(psoData_.rootParameters_.size());
	}
	/// <summary>
	/// Samplerの作成
	/// </summary>
	/// <param name="staticSamplers"></param>
	void CreateSampler(std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers) {
		psoData_.staticSamplers_ = staticSamplers;
		psoData_.descriptionRootSignature_.pStaticSamplers = psoData_.staticSamplers_.data();
		psoData_.descriptionRootSignature_.NumStaticSamplers = static_cast<UINT>(psoData_.staticSamplers_.size());
	}
	/// <summary>
	/// InputElementの作成
	/// </summary>
	/// <param name="inputElementDescs"></param>
	void CreateInputElement(std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs) {
		psoData_.inputElementDescs_ = inputElementDescs;
		psoData_.inputLayoutDesc_.pInputElementDescs = psoData_.inputElementDescs_.data();
		psoData_.inputLayoutDesc_.NumElements = static_cast<UINT>(psoData_.inputElementDescs_.size());
	}
	/// <summary>
	/// BlendStateの作成
	/// </summary>
	/// <param name="blendDesc"></param>
	void CreateBlendState(D3D12_BLEND_DESC blendDesc) { psoData_.blendDesc_ = blendDesc; }
	/// <summary>
	/// Rasterizerの作成
	/// </summary>
	/// <param name="rasterizerDesc"></param>
	void CreateRasterizer(D3D12_RASTERIZER_DESC rasterizerDesc) { psoData_.rasterizerDesc_ = rasterizerDesc; }

	/// <summary>
	/// 詳細な設定を適用する
	/// </summary>
	void ApplyDetailedSettings();

	/// <summary>
	/// 描画時の形状設定
	/// </summary>
	/// <param name="primitiveTopology">形状</param>
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) { psoData_.primitiveTopology_ = primitiveTopology; }
#pragma endregion

	// シェーダのコンパイル
	IDxcBlob* CompileShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile,
		// 初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

#pragma region Getter
	/// <summary>
	/// PSOの全データを取得
	/// </summary>
	/// <returns></returns>
	PSOData GetPSOData() { return psoData_; }
	/// <summary>
	/// DescriptorRangeの取得
	/// </summary>
	std::vector<D3D12_DESCRIPTOR_RANGE> GetDescriptorRange() { return psoData_.descriptorRange_; }
	/// <summary>
	/// ルートパラメータの取得
	/// </summary>
	std::vector<D3D12_ROOT_PARAMETER> GetRootParameter() { return psoData_.rootParameters_; }
	/// <summary>
	/// Samplerの取得
	/// </summary>
	std::vector<D3D12_STATIC_SAMPLER_DESC> GetSampler() { return psoData_.staticSamplers_; }
	/// <summary>
	/// InputElementの取得
	/// </summary>
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputElement() { return psoData_.inputElementDescs_; }
	/// <summary>
	/// BlendStateの取得
	/// </summary>
	D3D12_BLEND_DESC GetBlendState() { return psoData_.blendDesc_; }
	/// <summary>
	/// Rasterizerの取得
	/// </summary>
	D3D12_RASTERIZER_DESC GetRasterizer() { return psoData_.rasterizerDesc_; }
#pragma endregion

#pragma region Setter
	/// <summary>
	/// PSOのデータを設定
	/// </summary>
	/// <param name="data"></param>
	void SetPSOData(PSOData data) { 
		psoData_.descriptorRange_ = data.descriptorRange_; 
		
		psoData_ = data; 
		psoData_ = data; 
		psoData_.blendDesc_ = data.blendDesc_; 
		psoData_.inputElementDescs_ = data.inputElementDescs_; 
		psoData_.staticSamplers_ = data.staticSamplers_; 
	}
	/// <summary>
	/// コマンドを積む
	/// </summary>
	/// <param name="primitiveTopology"></param>
	/// <param name="resource"></param>
	virtual void SetCommand(Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr) = 0;
#pragma endregion

protected:
	// シェーダファイルまでのパス
	const std::wstring& kDirectoryPath = L"engine/resources/sharder/";

protected:
	// directXCommonのインスタンス
	DirectXCommon* dxCommon_;

	PSOData psoData_;
};