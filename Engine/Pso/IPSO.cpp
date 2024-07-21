#include "IPSO.h"
#include "ConvertString.h"
#include <format>
#include <cassert>

void IPSO::Init(IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, const std::string& VS_fileName, const std::string& PS_fileName) {
	// directX
	dxCommon_ = DirectXCommon::GetInstance();
	// Pipelinemanager
	psoData_.dxcUtils_ = dxcUtils;
	psoData_.dxcCompiler_ = dxcCompiler;
	psoData_.includeHandler_ = includeHandler;
	// VSのファイル名
	psoData_.VSFilePath_ = kDirectoryPath + ConvertString(VS_fileName);
	// PSのファイル名
	psoData_.PSFilePath_ = kDirectoryPath + ConvertString(PS_fileName);
}

void IPSO::ApplyDetailedSettings() {
	// シリアライズしてバイナリにする
	HRESULT hr;
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

IDxcBlob* IPSO::CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile,
	// 初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler) {

#pragma region hlslファイルを読む

	// これからシェーダーをコンパイラする旨をログに出す
	WinApp::Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

#pragma endregion

#pragma region Compileする

	LPCWSTR arguments[] = {
		filePath.c_str(), // コンパイル対象のhlslファイル名
		L"-E", L"main", // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile, // ShaderProfileの設定
		L"-Zi", L"-Qembed_debug", // デバッグ用の情報を埋め込む
		L"-Od", // 最適化を外しておく
		L"-Zpr", // メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,	// 読み込んだファイル
		arguments,				// コンパイルオプション
		_countof(arguments),	// コンパイルオプションの数
		includeHandler,			// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region 警告・エラーが出てないか確認する

	// 警告・エラーが出てきたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		WinApp::Log(shaderError->GetStringPointer());
		assert(false);
	}

#pragma endregion

#pragma region Compile結果を受け取って渡す

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	WinApp::Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを変換
	return shaderBlob;

#pragma endregion
}
