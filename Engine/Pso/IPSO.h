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
	void CreateDescriptorRange(std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRange) { descriptorRange_ = descriptorRange; }
	/// <summary>
	/// ルートパラメータの作成
	/// </summary>
	/// <param name="rootParameters"></param>
	void CreateRootParameter(std::vector<D3D12_ROOT_PARAMETER> rootParameters) {
		rootParameters = rootParameters;
		// rootParameterの設定を入れる
		descriptionRootSignature_.pParameters = rootParameters_.data();
		descriptionRootSignature_.NumParameters = static_cast<UINT>(rootParameters_.size());
	}
	/// <summary>
	/// Samplerの作成
	/// </summary>
	/// <param name="staticSamplers"></param>
	void CreateSampler(std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers) {
		staticSamplers_ = staticSamplers;
		descriptionRootSignature_.pStaticSamplers = staticSamplers_.data();
		descriptionRootSignature_.NumStaticSamplers = static_cast<UINT>(staticSamplers_.size());
	}
	/// <summary>
	/// InputElementの作成
	/// </summary>
	/// <param name="inputElementDescs"></param>
	void CreateInputElement(std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs) {
		inputElementDescs_ = inputElementDescs;
		inputLayoutDesc_.pInputElementDescs = inputElementDescs_.data();
		inputLayoutDesc_.NumElements = static_cast<UINT>(inputElementDescs_.size());
	}
	/// <summary>
	/// BlendStateの作成
	/// </summary>
	/// <param name="blendDesc"></param>
	void CreateBlendState(D3D12_BLEND_DESC blendDesc) { blendDesc_ = blendDesc; }
	/// <summary>
	/// Rasterizerの作成
	/// </summary>
	/// <param name="rasterizerDesc"></param>
	void CreateRasterizer(D3D12_RASTERIZER_DESC rasterizerDesc) { rasterizerDesc_ = rasterizerDesc; }

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

	/// Setter
	// コマンドを積む
	virtual void SetCommand(Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr) {
		// シグネチャの設定
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		// PSOを設定
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		// 形状を設定
		dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	};

protected:
	// シェーダファイルまでのパス
	const std::wstring& kDirectoryPath = L"engine/resources/sharder/";

protected:
	// directXCommonのインスタンス
	DirectXCommon* dxCommon_;

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
};