#include "SkinningPSO.h"

SkinningPSO* SkinningPSO::GetInstance() {
	static SkinningPSO instance;

	return &instance;
}

void SkinningPSO::Init(IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, const std::string& VS_fileName, const std::string& PS_fileName) {
	// 基底クラスの初期化
	IPSO::Init(dxcUtils, dxcCompiler, includeHandler, VS_fileName, PS_fileName);
}

void SkinningPSO::CreateRootSignature() {
	HRESULT hr;
	descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma region descriptorRange
	descriptorRange_.resize(1);
	descriptorRange_[0].BaseShaderRegister = 0;
	descriptorRange_[0].NumDescriptors = 1;
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
#pragma endregion

#pragma region rootParameter
	rootParameters_.resize(9);
#pragma region VSShaderに送るデータ
	// worldTransform
	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters_[1].Descriptor.ShaderRegister = 0;
	// viewProjection
	rootParameters_[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters_[4].Descriptor.ShaderRegister = 1;
	// matrixPalette
	rootParameters_[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters_[8].Descriptor.ShaderRegister = 0;
	rootParameters_[8].DescriptorTable.pDescriptorRanges = descriptorRange_.data();
	rootParameters_[8].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(descriptorRange_.size());
#pragma endregion

#pragma region PSShaderに送るデータ
	// material
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[0].Descriptor.ShaderRegister = 0;
	// texture
	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[2].DescriptorTable.pDescriptorRanges = descriptorRange_.data();
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(descriptorRange_.size());
	// 平行光源
	rootParameters_[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[3].Descriptor.ShaderRegister = 1;
	// カメラ位置
	rootParameters_[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[5].Descriptor.ShaderRegister = 2;
	// 点光源
	rootParameters_[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[6].Descriptor.ShaderRegister = 3;
	// スポットライト
	rootParameters_[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[7].Descriptor.ShaderRegister = 4;
#pragma endregion

#pragma endregion

	// rootParameterの設定を入れる
	descriptionRootSignature_.pParameters = rootParameters_.data();
	descriptionRootSignature_.NumParameters = static_cast<UINT>(rootParameters_.size());

#pragma region sampler
	staticSamplers_.resize(1);
	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers_[0].ShaderRegister = 0;
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers_.data();
	descriptionRootSignature_.NumStaticSamplers = static_cast<UINT>(staticSamplers_.size());
#pragma endregion

	// シリアライズしてバイナリにする
	hr = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr)) {
		WinApp::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	// バイナリをもとに生成
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void SkinningPSO::CreatePSO() {
	HRESULT hr;

	// rootSignatureの作成
	CreateRootSignature();

#pragma region inputElement
	inputElementDescs_.resize(5);
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[3].SemanticName = "WEIGHT";
	inputElementDescs_[3].SemanticIndex = 0;
	inputElementDescs_[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[3].InputSlot = 1;
	inputElementDescs_[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[4].SemanticName = "INDEX";
	inputElementDescs_[4].SemanticIndex = 0;
	inputElementDescs_[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	inputElementDescs_[4].InputSlot = 1;
	inputElementDescs_[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_.data();
	inputLayoutDesc_.NumElements = static_cast<UINT>(inputElementDescs_.size());
#pragma endregion

#pragma region blendState
	blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = true;
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドモードの設定
	// ノーマル
	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
#pragma endregion

#pragma region rasterizer
	// 裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
#pragma endregion

#pragma region シェーダのコンパイル
	// VS
	vertexShaderBlob_ = CompileShader(VSFilePath_,
		L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(vertexShaderBlob_ != nullptr);
	// PS
	pixelShaderBlob_ = CompileShader(PSFilePath_,
		L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob_ != nullptr);
#pragma endregion

	// rootSignature
	graphicsPipelineStateDescs_.pRootSignature = rootSignature_.Get();
	// InputLayout
	graphicsPipelineStateDescs_.InputLayout = inputLayoutDesc_;
	// vertexShader
	graphicsPipelineStateDescs_.VS = { vertexShaderBlob_->GetBufferPointer(),
	vertexShaderBlob_->GetBufferSize() };
	// pixelShader
	graphicsPipelineStateDescs_.PS = { pixelShaderBlob_->GetBufferPointer(),
	pixelShaderBlob_->GetBufferSize() };
	// DepthStencilの設定
	graphicsPipelineStateDescs_.DepthStencilState = dxCommon_->GetDepthStencilDesc();
	graphicsPipelineStateDescs_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// blendState
	graphicsPipelineStateDescs_.BlendState = blendDesc_;
	// rasterizerState
	graphicsPipelineStateDescs_.RasterizerState = rasterizerDesc_;
	// 書き込むRTVの情報
	graphicsPipelineStateDescs_.NumRenderTargets = 1;
	graphicsPipelineStateDescs_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロ時（形状）のタイプ。三角形
	graphicsPipelineStateDescs_.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくてよい）
	graphicsPipelineStateDescs_.SampleDesc.Count = 1;
	graphicsPipelineStateDescs_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDescs_,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}