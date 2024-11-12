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

	// レベルマネージャーの生成
	levelManager_ = LevelManager::GetInstance();

	// 衝突マネージャーを作成
	collisionManager_ = CollisionManager::GetInstance();

	//初期シーンの設定
	sceneNum_ = TITLE_SCENE;
	// シーンごとの初期化
	sceneArr_[sceneNum_]->Initialize();

	// シーン切り替え
	sceneTransition_ = SceneTransition::GetInstance();
	sceneTransition_->Initialize();
}

void GameManager::Update() {
	Framework::Update();

	// シーン切り替えの演出
	sceneTransition_->Update();

	// 現在のシーンをチェック
	preSceneNum_ = sceneNum_;
	sceneNum_ = sceneArr_[sceneNum_]->GetSceneNum();

	// シーン変更チェック
	if (sceneNum_ != preSceneNum_) {
		// Blenderで読み込んだオブジェクトを初期化
		levelManager_->Finalize();

		// コライダーリストをすべて削除
		collisionManager_->ClearColliderList();

		// ポストエフェクトを使用しないスプライトリストを消す
		postEffectManager_->ClearSpriteList();
		// ポストエフェクト機能を停止する
		postEffectManager_->ResetPostEffect();

		// 以前のシーンを解放する
		sceneArr_[preSceneNum_]->Finalize();
		sceneArr_[preSceneNum_].reset();
		// 新しいシーンを初期化
		sceneArr_[sceneNum_]->Initialize();

		// シーン遷移演出開始
		sceneTransition_->SetSceneTransitionSignal(false);
		// シーン遷移で使うスプライトはポストエフェクトを使用しない
		sceneTransition_->SetSpriteList();

		// 解放したシーンを作成しなおす
		switch (preSceneNum_) {
		case TITLE_SCENE:
			sceneArr_[TITLE_SCENE] = std::make_unique<TitleScene>();
			break;
		case GAME_SCENE:
			sceneArr_[GAME_SCENE] = std::make_unique<GameScene>();
			break;
		case GAMECLEAR_SCENE:
			sceneArr_[GAMECLEAR_SCENE] = std::make_unique<GameClear>();
			break;
		case GAMEOVER_SCENE:
			sceneArr_[GAMEOVER_SCENE] = std::make_unique<GameOver>();
			break;
		}
	}

	///
	/// 更新処理
	/// 
	sceneArr_[sceneNum_]->Update();

	// 衝突マネージャー(当たり判定)
	collisionManager_->CheckAllCollisions();

#ifdef _DEBUG
	// FPSカウンターの表示
	ImGui::Begin("Control panel");
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();
#endif
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
	// エンジン機能
	Framework::Finalize();

	// レベルマネージャー
	levelManager_->Finalize();

	// 各シーン
	for (auto& scene : sceneArr_) {
		scene->Finalize();
		scene.reset();
	}
}