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
	AddModel("PlayerBody", modelManager_->GetModel("Models", "Bob.obj"));
	// 自機の弾
	AddModel("PlayerBullet", modelManager_->GetModel("Models", "block.obj"));
	// デバッグ用
	// ロックオン時のマーカー用オブジェクト
	AddModel("RockOnObject", modelManager_->GetModel("Models", "block.obj"));
	// 通常敵のモデル
	AddModel("EnemyBody", modelManager_->GetModel("Models", "Spitfire.obj"));
	// 通常敵の弾
	AddModel("EnemyBullet", modelManager_->GetModel("Models", "block.obj"));
	// 天球
	AddModel("Skydome", modelManager_->GetModel("Level", "skydome.obj"));

	// Blender
	//levelManager_->LoadJSONFile("TestGoalMap.json", &camera_);
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
	player_.AddModel("PlayerBody",models_["PlayerBody"]);
	// 必要なクラスのアドレスをセットする
	player_.SetCamera(followCamera_.GetCamera());
	player_.SetGameScene(this);
	player_.Initialize();
	// 自キャラとレールカメラの親子関係を結ぶ
	player_.SetParent(&railCamera_.GetWorldTransform());
#pragma endregion

#pragma region エネミーマネージャの生成
	// 通常敵の体
	enemyManager_.AddModel("EnemyBody",models_["EnemyBody"]);
	// 通常敵の弾
	enemyManager_.AddModel("EnemyBullet",models_["EnemyBullet"]);
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
	multiLockOnSystem_->Initialize(&player_, followCamera_.GetCamera(), enemyManager_.GetEnemyList_P(), this, models_["RockOnObject"]);
	multiLockOnSystem_->SetEnemyList(enemyManager_.GetEnemyList_P());
	multiLockOnSystem_->SetEnemyIdList(enemyManager_.GetIdList_P());

	// 天球の生成
	skydome_.Initialize(models_["Skydome"], followCamera_.GetCamera(), railCamera_.GetWorldTransform_P());

	// スタート演出
	startEvent_ = std::make_unique<StartEvent>();
	startEvent_->Initialize(&player_, &followCamera_);
	// クリア演出
	clearEvent_ = std::make_unique<ClearEvent>();
	clearEvent_->Initialize(&player_, &followCamera_);
	// 死亡演出
	deadEvent_ = std::make_unique<DeadEvent>();
	deadEvent_->Initialize(&player_, &followCamera_, &railCamera_);

	// レールカメラ
	railCamera_.Update();
	// 追従カメラ
	followCamera_.Update();

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
}

void GameSystem::Update(int& sceneNum) {
	// スタート演出
	startEvent_->Update();
	// クリア演出
	clearEvent_->Update();
	// 死亡演出
	deadEvent_->Update();

	// シーン切り替えの条件
	SceneChange(sceneNum);

	// エネミーマネージャ
	enemyManager_.Update();

	// マルチロックオン
	multiLockOnSystem_->Update();

	// スタート演出中、クリア演出中, 死亡演出中は処理しない
	if (startEvent_->GetIsActive() && !startEvent_->GetIsEnd() || clearEvent_->GetIsActive() || clearEvent_->GetIsEnd() || deadEvent_->GetIsActive() || deadEvent_->GetIsEnd()) { return; }

	// スタート演出終了したら追従カメラの親子関係をもとに戻す
	if (startEvent_->GetIsEnd() && !clearEvent_->GetIsEnd()) {
		railCamera_.SetIsMove(true);
		followCamera_.SetParent(&railCamera_.GetWorldTransform());
	}

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

	// 敵に関するパーティクル
	enemyManager_.DrawParticle();

	// 操作UIを非表示にする
	for (Sprite& UI : guideUI_) {
		UI.isActive_ = false;
	}

	// スコアを非表示にする
	score_->SetIsDraw(false);

	// 開始演出、クリア演出、死亡演出中はボタンやゲージのUIを表示しない
	if (startEvent_->GetIsActive() || clearEvent_->GetIsActive() || clearEvent_->GetIsEnd() || deadEvent_->GetIsActive() || deadEvent_->GetIsEnd()) { return; }

	// 操作UI
	for (Sprite& UI : guideUI_) {
		UI.isActive_ = true;
	}

	// ロックオン可能状態なら攻撃UIを明るく表示
	if (multiLockOnSystem_->GetIsActive()) {
		guideUI_[0].SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });
		guideUI_[1].SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });
	}
	else {
		guideUI_[0].SetColor(Vector4{ 0.2f,0.2f,0.2f,1.0f });
		guideUI_[1].SetColor(Vector4{ 0.2f,0.2f,0.2f,1.0f });
	}
	// ジャスト回避のチュートリアルが終わるまで回避UIを出さない
	if (player_.GetFirstJustEvasionState() != 0) {
		guideUI_[4].isActive_ = true;
		guideUI_[5].isActive_ = true;
	}
	else {
		guideUI_[4].isActive_ = false;
		guideUI_[5].isActive_ = false;
	}

	// スコアを表示する
	score_->SetIsDraw(true);

	// 自機のレティクルとHP
	player_.DrawUI();

	// レールカメラの移動ルート
	//railCamera_.MoveRouteDraw();
}

void GameSystem::SceneChange(int& sceneNum) {
	// 死亡演出開始
	if (player_.GetIsDead()) {
		deadEvent_->Start();
	}
	// クリア演出開始
	else if (railCamera_.GetIsGameClear()) {
		clearEvent_->Start();
		railCamera_.SetIsMove(false);
	}

	// ゲームオーバ演出がすべて終了したらシーン遷移演出開始
	if (deadEvent_->GetIsEnd()) {
		SceneTransition::GetInstance()->Start();
	}
	// クリア演出が終了したらシーン遷移演出開始
	if (clearEvent_->GetIsEnd()) {
		SceneTransition::GetInstance()->Start();
	}

	// シーン切り替え可能信号が出てないなら遷移しない
	if (!SceneTransition::GetInstance()->GetSceneTransitionSignal()) { return; }

	// クリア条件
	if (clearEvent_->GetIsEnd()) {
		sceneNum = TITLE_SCENE;
	}
	// ゲームオーバ条件
	if (deadEvent_->GetIsEnd()) {
		sceneNum = TITLE_SCENE;
	}
}

void GameSystem::EffectUpdate() {
	// 演出の状態を決定する
	effectState_ = Normal;

	// 初めてジャスト回避を行うとき
	if (player_.GetFirstJustEvasion()) {
		effectState_ = FirstJustEvasion;
	}
	// ジャスト回避中
	else if (player_.GetEvasionData().isJust) {
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
		lightDecay_ = Lerps::ExponentialInterpolate(lightDecay_, 1.0f, 0.7f);
		PointLight::GetInstance()->SetDecay(lightDecay_);

		// ラジアルブラーを消す
		blurStrength_ = Lerps::ExponentialInterpolate(blurStrength_, 0.0f, 0.5f);
		PostEffectManager::GetInstance()->radialBlurData_.blurWidth = blurStrength_;
		if (blurStrength_ <= 0.0f) {
			PostEffectManager::GetInstance()->radialBlurData_.isActive = false;
		}

		// ヴィネットを消す		
		vignetteScale_ = Lerps::ExponentialInterpolate(vignetteScale_, kDefaultVignetteScale, 0.1f);
		PostEffectManager::GetInstance()->vignetingData_.scale = vignetteScale_;
		if (vignetteScale_ >= kDefaultVignetteScale - 1) {
			PostEffectManager::GetInstance()->vignetingData_.isActive = false;
		}

		// コントローラーの振動を消す
		Input::GetInstance()->GamePadVibration(0, 0, 0);

		// 時間の速さを戻す
		timeScale_ = Lerps::ExponentialInterpolate(timeScale_, 1.0f, 0.5f);
		GameTimer::GetInstance()->SetTimeScale(timeScale_);
		break;
	case FirstJustEvasion:
		// 暗転
		lightDecay_ = Lerps::ExponentialInterpolate(lightDecay_, kMaxPointLightDecay, 0.01f);
		PointLight::GetInstance()->SetDecay(lightDecay_);

		// ラジアルブラーを消す
		blurStrength_ = Lerps::ExponentialInterpolate(blurStrength_, 0.0f, 0.5f);
		PostEffectManager::GetInstance()->radialBlurData_.blurWidth = blurStrength_;
		if (blurStrength_ <= 0.0f) {
			PostEffectManager::GetInstance()->radialBlurData_.isActive = false;
		}

		// ヴィネットを消す		
		vignetteScale_ = Lerps::ExponentialInterpolate(vignetteScale_, kDefaultVignetteScale, 0.1f);
		PostEffectManager::GetInstance()->vignetingData_.scale = vignetteScale_;
		if (vignetteScale_ >= kDefaultVignetteScale - 1) {
			PostEffectManager::GetInstance()->vignetingData_.isActive = false;
		}

		// コントローラーの振動を消す
		Input::GetInstance()->GamePadVibration(0, 0, 0);

		// 時間の速さを遅くする
		timeScale_ = Lerps::ExponentialInterpolate(timeScale_, 0.0f, 0.8f);
		GameTimer::GetInstance()->SetTimeScale(timeScale_);
		break;
	case JustEvasion:
		// ライトの減衰率を戻す
		lightDecay_ = Lerps::ExponentialInterpolate(lightDecay_, 1.0f, 0.7f);
		PointLight::GetInstance()->SetDecay(lightDecay_);

		// ラジアルブラーをかける
		PostEffectManager::GetInstance()->radialBlurData_.isActive = true;
		blurStrength_ = Lerps::ExponentialInterpolate(blurStrength_, kBlurStrength, 0.1f);
		PostEffectManager::GetInstance()->radialBlurData_.blurWidth = blurStrength_;

		// ヴィネットをかける
		PostEffectManager::GetInstance()->vignetingData_.isActive = true;
		vignetteScale_ = Lerps::ExponentialInterpolate(vignetteScale_, kMaxVignetteScale + 200, 0.1f);
		PostEffectManager::GetInstance()->vignetingData_.scale = vignetteScale_;

		// コントローラーの振動を消す
		Input::GetInstance()->GamePadVibration(0, 0, 0);

		// 時間の速さを遅くする
		timeScale_ = Lerps::ExponentialInterpolate(timeScale_, 0.1f, 0.1f);
		GameTimer::GetInstance()->SetTimeScale(timeScale_);
		break;
	case Boost:
		// ライトの減衰率を戻す
		lightDecay_ = Lerps::ExponentialInterpolate(lightDecay_, 1.0f, 0.7f);
		PointLight::GetInstance()->SetDecay(lightDecay_);

		// ラジアルブラーをかける
		PostEffectManager::GetInstance()->radialBlurData_.isActive = true;
		blurStrength_ = Lerps::ExponentialInterpolate(blurStrength_, kBlurStrength, 0.1f);
		PostEffectManager::GetInstance()->radialBlurData_.blurWidth = blurStrength_;

		// ヴィネットをかける
		PostEffectManager::GetInstance()->vignetingData_.isActive = true;
		vignetteScale_ = Lerps::ExponentialInterpolate(vignetteScale_, kMaxVignetteScale, 0.1f);
		PostEffectManager::GetInstance()->vignetingData_.scale = vignetteScale_;

		// コントローラーを振動させる
		Input::GetInstance()->GamePadVibration(0, 65535, 65535);

		// 時間の速さを戻す
		timeScale_ = Lerps::ExponentialInterpolate(timeScale_, 1.0f, 0.5f);
		GameTimer::GetInstance()->SetTimeScale(timeScale_);
		break;
	}
}
