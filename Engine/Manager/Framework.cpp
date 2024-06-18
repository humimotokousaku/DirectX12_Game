#include "Framework.h"
#include "ConvertString.h"
#include "GlobalVariables.h"
#include "GameTime.h"

Framework::Framework() {

}

void Framework::Initialize() {
	// タイトル名を入力
	const char kWindowTitle[] = "GE3_CLASS";
	// タイトルバーの変換
	auto&& titleString = ConvertString(kWindowTitle);

	/// エンジン機能の生成
	// windowの初期化
	winApp_ = WinApp::GetInstance();
	winApp_->Initialize(titleString.c_str(), 1280, 720);
	// DirectXの初期化
	directXCommon_ = DirectXCommon::GetInstance();
	directXCommon_->Initialize(winApp_->GetHwnd());

	// SRVマネージャの初期化
	srvManager_ = SrvManager::GetInstance();
	srvManager_->Initialize();
	// ImGuiの初期化
	imGuiManager_ = new ImGuiManager();
	imGuiManager_->Initialize(winApp_->GetHwnd());
	// Textureの初期化
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize(srvManager_);
	// モデルマネージャ
	ModelManager::GetInstance()->Initialize();

	// ブローバル変数の読み込み
	//GlobalVariables::GetInstance()->LoadFiles();
	
	// PSOの初期化
	pipelineManager_ = PipelineManager::GetInstance();
	pipelineManager_->Initialize();
	// 線のPSO

	// ポストエフェクトのPSO
	//postEffectPSO_ = PostEffectPSO::GetInstance();
	//postEffectPSO_->Initialize();



	// ポストエフェクト
	postEffectManager_ = new PostEffectManager();
	postEffectManager_->Initialize();

	/// Components
	// 入力(キーボードとゲームパッド)
	input_ = Input::GetInstance();
	input_->Initialize();
	// ライトの設定
	directionalLight_ = DirectionalLight::GetInstance();
	directionalLight_->Initialize();
	// 点光源
	pointLight_ = PointLight::GetInstance();
	pointLight_->Initialize();
	// スポットライト
	spotLight_ = SpotLight::GetInstance();
	spotLight_->Initialize();
	// Audioの初期化
	Audio::GetInstance()->Initialize();
}

void Framework::Update() {
	GameTimer::GetInstance()->Tick();
	//pointLight_->ImGuiAdjustParameter();
}

void Framework::Run() {
	// 初期化
	Initialize();

	MSG msg{};
	// ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		// Windowにメッセージが来ていたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// 描画前の処理
			BeginFrame();

			// 更新処理
			Update();

			// 出力結果をテクスチャにする
			postEffectManager_->PreDraw();
			// 描画処理
			Draw();
			postEffectManager_->PostDraw();
			
			directXCommon_->PreDraw();
			// 2パス目を描画
			postEffectManager_->Draw();
			// 描画後の処理
			EndFrame();
		}
	}
	// 解放処理
	Finalize();
}

void Framework::Finalize() {
	audio_->Finalize();
	//audio_->SoundUnload(&soundData1_);
	ModelManager::GetInstance()->Finalize();
	// ImGui
	imGuiManager_->Finalize();
	delete postEffectManager_;
	textureManager_->Finalize();
	//delete srvManager_;
	directXCommon_->Finalize();
	CloseWindow(winApp_->GetHwnd());
	// Textureのゲーム終了処理
	textureManager_->ComUninit();
}

void Framework::BeginFrame() {
	// キーの状態を取得
	input_->Update();
	// SrvManager
	srvManager_->PreDraw();
	// PSO
	pipelineManager_->PreDraw();
	// ImGui
	imGuiManager_->PreDraw();
	// グローバル変数の更新
	//GlobalVariables::GetInstance()->Update();
}

void Framework::EndFrame() {
	// ImGui
	imGuiManager_->PostDraw();
	// DirectXCommon
	directXCommon_->PostDraw();
}