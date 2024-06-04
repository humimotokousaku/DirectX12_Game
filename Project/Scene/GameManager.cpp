#include "GameManager.h"

GameManager::GameManager() {
	// 各シーンの配列
	sceneArr_[TITLE_SCENE] = std::make_unique<TitleScene>();
	sceneArr_[GAME_SCENE] = std::make_unique<GameScene>();
	sceneArr_[GAMECLEAR_SCENE] = std::make_unique<GameClear>();
	sceneArr_[GAMEOVER_SCENE] = std::make_unique<GameOver>();
}

GameManager::~GameManager() {
	Framework::~Framework();
}

void GameManager::Initialize() {
	Framework::Initialize();
	//初期シーンの設定
	sceneNum_ = TITLE_SCENE;
	// シーンごとの初期化
	sceneArr_[sceneNum_]->Initialize();
}

void GameManager::Update() {
	Framework::Update();

	// シーンチェック
	preSceneNum_ = sceneNum_;
	sceneNum_ = sceneArr_[sceneNum_]->GetSceneNum();

	//シーン変更チェック
	if (sceneNum_ != preSceneNum_) {
		sceneArr_[preSceneNum_]->Finalize();
		sceneArr_[sceneNum_]->Initialize();
	}

	///
	/// 更新処理
	/// 
	sceneArr_[sceneNum_]->Update();

#ifdef USE_IMGUI
	// FPSカウンターの表示
	ImGui::Begin("Control panel");
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();
#endif // USE_IMGUI
}

void GameManager::Draw() {
	sceneArr_[sceneNum_]->Draw();
}

void GameManager::BeginFrame() {
	Framework::BeginFrame();
}

void GameManager::EndFrame() {
	Framework::EndFrame();
}

void GameManager::Finalize() {
	Framework::Finalize();
	for (auto& scene : sceneArr_) {
		scene->Finalize();
		scene.reset();
	}
}