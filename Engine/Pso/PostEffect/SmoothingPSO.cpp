#include "SmoothingPSO.h"

SmoothingPSO::SmoothingPSO(IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, const std::string& VS_fileName, const std::string& PS_fileName) {
	// 初期化
	Init(dxcUtils, dxcCompiler, includeHandler, VS_fileName, PS_fileName);
}

void SmoothingPSO::Init(IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, const std::string& VS_fileName, const std::string& PS_fileName) {
	// 基底クラスの初期化
	IPSO::Init(dxcUtils, dxcCompiler, includeHandler, VS_fileName, PS_fileName);
	// PSO作成
	CreatePSO();
}

void SmoothingPSO::CreateRootSignature() {
	HRESULT hr;
	psoData_.descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma region descriptorRange
	psoData_.descriptorRange_.resize(1);
	psoData_.descriptorRange_[0].BaseShaderRegister = 0;
	psoData_.descriptorRange_[0].NumDescriptors = 1;
	psoData_.descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	psoData_.descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
#pragma endregion

#pragma region rootParameter
	psoData_.rootParameters_.resize(4);
	// material
	psoData_.rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	psoData_.rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	psoData_.rootParameters_[0].Descriptor.ShaderRegister = 0;
	// texture
	psoData_.rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	psoData_.rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	psoData_.rootParameters_[1].DescriptorTable.pDescriptorRanges = psoData_.descriptorRange_.data();
	psoData_.rootParameters_[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(psoData_.descriptorRange_.size());
	// worldTransform
	psoData_.rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	psoData_.rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	psoData_.rootParameters_[2].Descriptor.ShaderRegister = 0;
	// viewProjection
	psoData_.rootParameters_[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	psoData_.rootParameters_[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	psoData_.rootParameters_[3].Descriptor.ShaderRegister = 1;
#pragma endregion

	// rootParameterの設定を入れる
	psoData_.descriptionRootSignature_.pParameters = psoData_.rootParameters_.data();
	psoData_.descriptionRootSignature_.NumParameters = static_cast<UINT>(psoData_.rootParameters_.size());

#pragma region sampler
	psoData_.staticSamplers_.resize(1);
	psoData_.staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	psoData_.staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	psoData_.staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	psoData_.staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	psoData_.staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	psoData_.staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;
	psoData_.staticSamplers_[0].ShaderRegister = 0;
	psoData_.staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	psoData_.descriptionRootSignature_.pStaticSamplers = psoData_.staticSamplers_.data();
	psoData_.descriptionRootSignature_.NumStaticSamplers = static_cast<UINT>(psoData_.staticSamplers_.size());
#pragma endregion

	// シリアライズしてバイナリにする
	hr = D3D12SerializeRootSignature(&psoData_.descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &psoData_.signatureBlob_, &psoData_.errorBlob_);
	if (FAILED(hr)) {
		WinApp::Log(reinterpret_cast<char*>(psoData_.errorBlob_->GetBufferPointer()));
		assert(false);
	}
	// バイナリをもとに生成
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, psoData_.signatureBlob_->GetBufferPointer(),
		psoData_.signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&psoData_.rootSignature_));
	assert(SUCCEEDED(hr));
}

void SmoothingPSO::CreatePSO() {
	HRESULT hr;

	// rootSignatureの作成
	CreateRootSignature();

#pragma region inputElement
	psoData_.inputElementDescs_.resize(2);
	psoData_.inputElementDescs_[0].SemanticName = "POSITION";
	psoData_.inputElementDescs_[0].SemanticIndex = 0;
	psoData_.inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	psoData_.inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	psoData_.inputElementDescs_[1].SemanticName = "TEXCOORD";
	psoData_.inputElementDescs_[1].SemanticIndex = 0;
	psoData_.inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	psoData_.inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	psoData_.inputLayoutDesc_.pInputElementDescs = psoData_.inputElementDescs_.data();
	psoData_.inputLayoutDesc_.NumElements = static_cast<UINT>(psoData_.inputElementDescs_.size());
#pragma endregion

#pragma region blendState
	psoData_.blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	// すべての色要素を書き込む
	psoData_.blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	psoData_.blendDesc_.RenderTarget[0].BlendEnable = true;
	psoData_.blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	psoData_.blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoData_.blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドモードの設定
	// ノーマル
	psoData_.blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoData_.blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoData_.blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
#pragma endregion

#pragma region rasterizer
	// 裏面(時計回り)を表示しない
	psoData_.rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	psoData_.rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
#pragma endregion

#pragma region シェーダのコンパイル
	// VS
	psoData_.vertexShaderBlob_ = CompileShader(psoData_.VSFilePath_,
		L"vs_6_0", psoData_.dxcUtils_, psoData_.dxcCompiler_, psoData_.includeHandler_);
	assert(psoData_.vertexShaderBlob_ != nullptr);
	// PS
	psoData_.pixelShaderBlob_ = CompileShader(psoData_.PSFilePath_,
		L"ps_6_0", psoData_.dxcUtils_, psoData_.dxcCompiler_, psoData_.includeHandler_);
	assert(psoData_.pixelShaderBlob_ != nullptr);
#pragma endregion

	// rootSignature
	psoData_.graphicsPipelineStateDescs_.pRootSignature = psoData_.rootSignature_.Get();
	// InputLayout
	psoData_.graphicsPipelineStateDescs_.InputLayout = psoData_.inputLayoutDesc_;
	// vertexShader
	psoData_.graphicsPipelineStateDescs_.VS = { psoData_.vertexShaderBlob_->GetBufferPointer(),
	psoData_.vertexShaderBlob_->GetBufferSize() };
	// pixelShader
	psoData_.graphicsPipelineStateDescs_.PS = { psoData_.pixelShaderBlob_->GetBufferPointer(),
	psoData_.pixelShaderBlob_->GetBufferSize() };
	// DepthStencilの設定
	psoData_.graphicsPipelineStateDescs_.DepthStencilState = dxCommon_->GetDepthStencilDesc();
	psoData_.graphicsPipelineStateDescs_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// blendState
	psoData_.graphicsPipelineStateDescs_.BlendState = psoData_.blendDesc_;
	// rasterizerState
	psoData_.graphicsPipelineStateDescs_.RasterizerState = psoData_.rasterizerDesc_;
	// 書き込むRTVの情報
	psoData_.graphicsPipelineStateDescs_.NumRenderTargets = 1;
	psoData_.graphicsPipelineStateDescs_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロ時（形状）のタイプ。三角形
	psoData_.graphicsPipelineStateDescs_.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくてよい）
	psoData_.graphicsPipelineStateDescs_.SampleDesc.Count = 1;
	psoData_.graphicsPipelineStateDescs_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&psoData_.graphicsPipelineStateDescs_,
		IID_PPV_ARGS(&psoData_.graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}