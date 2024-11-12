#include "GameSystem.h"
#include "SceneTransition/SceneTransition.h"
#include "IScene.h"

GameSystem::~GameSystem() {
	// 自機の弾
	for (PlayerBullet* bullet : playerBullets_) {
		delete bullet;
	}
	playerBullets_.clear();
}

void GameSystem::Initialize() {
	textureManager_ = TextureManager::GetInstance();
	modelManager_ = ModelManager::GetInstance();
	audio_ = Audio::GetInstance();
	levelManager_ = LevelManager::GetInstance();

	// カメラの生成
	camera_.Initialize();

#pragma region 読み込み
	// テクスチャの読み込み
	textureManager_->LoadTexture("", "reticle.png");
	textureManager_->LoadTexture("Textures", "Bob_Red.png");
	textureManager_->LoadTexture("Textures", "Spitfire_Purple.png");

	// BGMの読み込み
	BGM_ = audio_->SoundLoadWave("Audio/gameBGM_Noesis.wav");
	audio_->SoundPlayWave(BGM_, true, 0.1f);

	// 使用するモデルの読み込み
	modelManager_->LoadModel("", "block.obj");
	modelManager_->LoadModel("", "Bob.obj");
	modelManager_->LoadModel("", "Spitfire.obj");
	modelManager_->LoadModel("", "boostFire.obj");
	modelManager_->LoadModel("Level", "tail.obj");
	modelManager_->LoadModel("Level", "skydome.obj");

	// 自機
	AddModel(modelManager_->SetModel("", "Bob.obj"));
	// 3Dレティクル
	AddModel(modelManager_->SetModel("", "block.obj"));
	// 自機の弾
	AddModel(modelManager_->SetModel("", "block.obj"));
	// 通常敵のモデル
	AddModel(modelManager_->SetModel("", "Spitfire.obj"));
	// 通常敵の弾
	AddModel(modelManager_->SetModel("", "block.obj"));
	// 天球
	AddModel(modelManager_->SetModel("Level", "skydome.obj"));
	// ブーストの炎
	AddModel(modelManager_->SetModel("", "boostFire.obj"));

	// Blender
	levelManager_->LoadJSONFile("GameMap_04.json", &camera_);
#pragma endregion

	// スコアの生成
	score_ = Score::GetInstance();
	score_->Initialize();

#pragma region 追従カメラとレールカメラ生成
	// レールカメラの生成
	railCamera_.Initialize(levelManager_->GetRailCameraControlPoints(), &player_);
	// 追従カメラの生成
	followCamera_.Initialize(&player_);
	followCamera_.SetParent(&railCamera_.GetWorldTransform());
	followCamera_.SetFov(railCamera_.GetFov());

	// 使用するカメラの変更
	levelManager_->SetCamera(followCamera_.GetCamera());
#pragma endregion

#pragma region 自機の作成
	// 自機モデル
	player_.AddModel(models_[0]);
	// 3Dレティクルモデル
	player_.AddModel(models_[1]);
	// 弾モデル
	player_.AddModel(models_[2]);
	// 弾モデル
	player_.AddModel(models_[6]);
	// 必要なクラスのアドレスをセットする
	player_.SetCamera(followCamera_.GetCamera());
	player_.SetGameScene(this);
	player_.Initialize();
	// 自キャラとレールカメラの親子関係を結ぶ
	player_.SetParent(&railCamera_.GetWorldTransform());
#pragma endregion

#pragma region エネミーマネージャの生成
	// 通常敵の体
	enemyManager_.AddModel(models_[3]);
	// 通常敵の弾
	enemyManager_.AddModel(models_[4]);
	// アドレスを設定
	enemyManager_.SetCamera(followCamera_.GetCamera());
	enemyManager_.SetRailCamera(&railCamera_);
	enemyManager_.SetPlayer(&player_);
	enemyManager_.SetSpawnPoints(levelManager_->GetEnemyPoints());
	// 初期化
	enemyManager_.Initialize();
#pragma endregion

#pragma region 自機のロックオンクラス生成
	aimAssist_ = AimAssist::GetInstance();
	aimAssist_->SetCamera(followCamera_.GetCamera());
	aimAssist_->SetPlayer(&player_);
	// カメラの方向ベクトルをアドレスで渡す
	aimAssist_->SetCameraDirectionVelocity(railCamera_.GetDirectionVelocity());
	// 自機クラスにアドレスを渡す
	player_.SetIsLockOn(aimAssist_->GetIsLockOn());
	player_.SetLockOnReticleOffset(aimAssist_->GetLockOnReticleOffset());
#pragma endregion

	// 天球の生成
	skydome_.Initialize(models_[5], followCamera_.GetCamera(), railCamera_.GetWorldTransform_P());

#pragma region UIスプライトを作成
	guideUI_[0].Initialize("Textures/UI", "guide_Attack.png");
	guideUI_[0].SetAnchorPoint(Vector2{ 0.5f, 0.5f });
	guideUI_[0].SetPos(Vector2{ 1200.0f, 64.0f });
	guideUI_[0].SetSize(Vector2{ 64.0f, 64.0f });
	PostEffectManager::GetInstance()->AddSpriteList(&guideUI_[0]);
	guideUI_[1].Initialize("Textures/UI", "guide_pad_RB.png");
	guideUI_[1].SetAnchorPoint(Vector2{ 0.5f, 0.5f });
	guideUI_[1].SetPos(Vector2{ 1132.0f, 64.0f });
	PostEffectManager::GetInstance()->AddSpriteList(&guideUI_[1]);
	guideUI_[2].Initialize("Textures/UI", "guide_Boost.png");
	guideUI_[2].SetAnchorPoint(Vector2{ 0.5f, 0.5f });
	guideUI_[2].SetPos(Vector2{ 1200.0f, 128.0f });
	guideUI_[2].SetSize(Vector2{ 64.0f, 64.0f });
	PostEffectManager::GetInstance()->AddSpriteList(&guideUI_[2]);
	guideUI_[3].Initialize("Textures/UI", "guide_pad_X.png");
	guideUI_[3].SetSize(Vector2{ 32,32 });
	guideUI_[3].SetPos(Vector2{ 1132.0f, 128.0f });
	PostEffectManager::GetInstance()->AddSpriteList(&guideUI_[3]);
#pragma endregion

	//PostEffectManager::GetInstance()->bloomData_.isActive = true;
}

void GameSystem::Update(int& sceneNum) {
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		sceneNum = TITLE_SCENE;
	}
	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		sceneNum = GAMEOVER_SCENE;
	}
	if (Input::GetInstance()->TriggerKey(DIK_3)) {
		sceneNum = GAMECLEAR_SCENE;
	}
#endif // _DEBUG

	// エネミーマネージャ
	enemyManager_.Update();

	// 敵のリストを保存
	aimAssist_->SetEnemyList(enemyManager_.GetEnemyList());

	// 自キャラの更新
	player_.Update();
	// 終了した弾を削除
	playerBullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});
	// 弾の更新
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Update();
	}

	// レールカメラ
	railCamera_.Update();
	// 追従カメラ
	followCamera_.Update();

	// スコア
	score_->Update();

	// ステージBGM
	//audio_->SetMuffle(BGM_, 1.0f);

	// シーン切り替えの条件
	SceneChange(sceneNum);
}

void GameSystem::Draw() {
	// Blenderで配置したオブジェクト
	levelManager_->Draw();

	// 敵の体、弾を描画
	enemyManager_.Draw();

	// 天球
	skydome_.Draw();

	// 自機
	player_.Draw();
	// 自機の弾
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Draw();
	}
	// 自機のレティクルとHP
	player_.DrawUI();

	// 敵に関するパーティクル
	enemyManager_.DrawParticle();

	// レールカメラの移動ルート
	//railCamera_.MoveRouteDraw();
}

void GameSystem::SceneChange(int& sceneNum) {
	// クリア条件
	if (railCamera_.GetIsGameClear()) {
		SceneTransition::GetInstance()->Start();
	}
	// ゲームオーバ条件
	if (player_.GetIsDead()) {
		SceneTransition::GetInstance()->Start();
	}

	// シーン切り替え可能信号が出てないなら終了
	if (!SceneTransition::GetInstance()->GetSceneTransitionSignal()) { return; }

	// クリア条件
	if (railCamera_.GetIsGameClear()) {
		sceneNum = GAMECLEAR_SCENE;
	}
	// ゲームオーバ条件
	if (player_.GetIsDead()) {
		sceneNum = GAMEOVER_SCENE;
	}
}