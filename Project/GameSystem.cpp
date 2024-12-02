#include "GameSystem.h"
#include "SceneTransition/SceneTransition.h"
#include "IScene.h"
#include "PointLight.h"

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
	textureManager_->LoadTexture("Textures", "reticle.png");
	textureManager_->LoadTexture("Textures", "Bob_Red.png");
	textureManager_->LoadTexture("Textures", "Spitfire_Purple.png");

	// BGMの読み込み
	BGM_ = audio_->SoundLoadWave("Audio/gameBGM_Noesis.wav");
	audio_->SoundPlayWave(BGM_, true, 0.1f);

	// 使用するモデルの読み込み
	modelManager_->LoadModel("Models", "block.obj");
	modelManager_->LoadModel("Models", "Bob.obj");
	modelManager_->LoadModel("Models", "Spitfire.obj");
	modelManager_->LoadModel("Models", "boostFire.obj");
	modelManager_->LoadModel("Level", "tail.obj");
	modelManager_->LoadModel("Level", "skydome.obj");

	// 自機
	AddModel(modelManager_->GetModel("Models", "Bob.obj"));
	// 3Dレティクル
	AddModel(modelManager_->GetModel("Models", "block.obj"));
	// 自機の弾
	AddModel(modelManager_->GetModel("Models", "block.obj"));
	// 通常敵のモデル
	AddModel(modelManager_->GetModel("Models", "Spitfire.obj"));
	// 通常敵の弾
	AddModel(modelManager_->GetModel("Models", "block.obj"));
	// 天球
	AddModel(modelManager_->GetModel("Level", "skydome.obj"));
	// ブーストの炎
	AddModel(modelManager_->GetModel("Models", "boostFire.obj"));

	// Blender
	levelManager_->LoadJSONFile("JustMap_00.json", &camera_);
	//levelManager_->LoadJSONFile("GameMap_04.json", &camera_);
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

	// マルチロックオン機能
	multiLockOnSystem_ = std::make_unique<MultiLockOnSystem>();
	multiLockOnSystem_->Initialize(&player_, followCamera_.GetCamera(), enemyManager_.GetEnemyList_P(), this, models_[2]);
	multiLockOnSystem_->SetEnemyList(enemyManager_.GetEnemyList_P());
	multiLockOnSystem_->SetEnemyIdList(enemyManager_.GetIdList_P());

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
	guideUI_[4].Initialize("Textures/UI", "guide_Evasion.png");
	guideUI_[4].SetAnchorPoint(Vector2{ 0.5f, 0.5f });
	guideUI_[4].SetPos(Vector2{ 1200.0f, 192.0f });
	guideUI_[4].SetSize(Vector2{ 64.0f, 64.0f });
	PostEffectManager::GetInstance()->AddSpriteList(&guideUI_[4]);
	guideUI_[5].Initialize("Textures/UI", "guide_pad_A.png");
	guideUI_[5].SetSize(Vector2{ 32,32 });
	guideUI_[5].SetPos(Vector2{ 1132.0f, 192.0f });
	PostEffectManager::GetInstance()->AddSpriteList(&guideUI_[5]);
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

	// マルチロックオン
	multiLockOnSystem_->Update();

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

	// ポストエフェクトや暗転などの処理
	EffectUpdate();

	// シーン切り替えの条件
	SceneChange(sceneNum);
}

void GameSystem::Draw() {
	// Blenderで配置したオブジェクト
	levelManager_->Draw();

	// 敵の体、弾を描画
	enemyManager_.Draw();

	// ロックオン時のレティクル
	multiLockOnSystem_->Draw();

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
	if (player_.GetDeadAnimationData().isEnd) {
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

void GameSystem::EffectUpdate() {
	// 演出の状態を決定する
	effectState_ = Normal;

	// ジャスト回避中
	if (player_.GetEvasionData().isJust) {
		effectState_ = JustEvasion;
	}
	// 加速中
	else if (player_.GetIsBoost()) {
		effectState_ = Boost;
	}
	// 自機が死んでいるとき
	else if (player_.GetIsDead()) {
		effectState_ = Normal;
	}

	// 演出を切り替える
	switch (effectState_) {
	case Normal:
		// ライトの減衰率を戻す
		lightDecay_ = Lerps::ExponentialInterpolate(lightDecay_, 1.0f, 1.0f, 0.7f);
		PointLight::GetInstance()->SetDecay(lightDecay_);

		// ラジアルブラーを消す
		blurStrength_ = Lerps::ExponentialInterpolate(blurStrength_, 0.0f, 1.0f, 0.5f);
		PostEffectManager::GetInstance()->radialBlurData_.blurWidth = blurStrength_;
		if (blurStrength_ <= 0.0f) {
			PostEffectManager::GetInstance()->radialBlurData_.isActive = false;
		}

		// ヴィネットを消す		
		vignetteScale_ = Lerps::ExponentialInterpolate(vignetteScale_, kDefaultVignetteScale, 1.0f, 0.1f);
		PostEffectManager::GetInstance()->vignetingData_.scale = vignetteScale_;
		if (vignetteScale_ >= kDefaultVignetteScale - 1) {
			PostEffectManager::GetInstance()->vignetingData_.isActive = false;
		}

		// コントローラーの振動を消す
		Input::GetInstance()->GamePadVibration(0, 0, 0);

		// 時間の速さを戻す
		GameTimer::GetInstance()->SetTimeScale(1.0f);
		break;
	case JustEvasion:
		// 暗転
		//lightDecay_ = Lerps::ExponentialInterpolate(lightDecay_, kMaxPointLightDecay, 1.0f, 0.1f);
		//PointLight::GetInstance()->SetDecay(lightDecay_);

		// ラジアルブラーをかける
		PostEffectManager::GetInstance()->radialBlurData_.isActive = true;
		blurStrength_ = Lerps::ExponentialInterpolate(blurStrength_, kBlurStrength, 1.0f, 0.1f);
		PostEffectManager::GetInstance()->radialBlurData_.blurWidth = blurStrength_;
		
		// ヴィネットをかける
		PostEffectManager::GetInstance()->vignetingData_.isActive = true;
		vignetteScale_ = Lerps::ExponentialInterpolate(vignetteScale_, kMaxVignetteScale + 200, 1.0f, 0.1f);
		PostEffectManager::GetInstance()->vignetingData_.scale = vignetteScale_;

		// コントローラーの振動を消す
		Input::GetInstance()->GamePadVibration(0, 0, 0);

		// 時間の速さを遅くする
		GameTimer::GetInstance()->SetTimeScale(0.1f);
		break;
	case Boost:
		// ライトの減衰率を戻す
		lightDecay_ = Lerps::ExponentialInterpolate(lightDecay_, 1.0f, 1.0f, 0.7f);
		PointLight::GetInstance()->SetDecay(lightDecay_);

		// ラジアルブラーをかける
		PostEffectManager::GetInstance()->radialBlurData_.isActive = true;
		blurStrength_ = Lerps::ExponentialInterpolate(blurStrength_, kBlurStrength, 1.0f, 0.1f);
		PostEffectManager::GetInstance()->radialBlurData_.blurWidth = blurStrength_;

		// ヴィネットをかける
		PostEffectManager::GetInstance()->vignetingData_.isActive = true;
		vignetteScale_ = Lerps::ExponentialInterpolate(vignetteScale_, kMaxVignetteScale, 1.0f, 0.1f);
		PostEffectManager::GetInstance()->vignetingData_.scale = vignetteScale_;

		// コントローラーを振動させる
		Input::GetInstance()->GamePadVibration(0, 65535, 65535);

		// 時間の速さを戻す
		GameTimer::GetInstance()->SetTimeScale(1.0f);
		break;
	}
}
