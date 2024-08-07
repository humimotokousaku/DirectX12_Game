#include "GameScene.h"
#include "GameManager.h"

void GameScene::Initialize() {
	sceneNum = GAME_SCENE;
	textureManager_ = TextureManager::GetInstance();
	modelManager_ = ModelManager::GetInstance();
	audio_ = Audio::GetInstance();

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

	// Blender
	LoadJSONFile("GameMap_00.json");
#pragma endregion

	// カメラの生成
	//camera_ = std::make_unique<Camera>();
	camera_.Initialize();
	// カメラレールの生成
	//railCamera_ = std::make_unique<RailCamera>();
	railCamera_.Initialize(controlPoints_);
	// 追従カメラの生成
	//followCamera_ = std::make_unique<FollowCamera>();
	followCamera_.Initialize();

	// 衝突マネージャーの生成
	//collisionManager_ = std::make_unique<CollisionManager>();

	// 自キャラの生成
	//player_ = std::make_unique<Player>();
	// 自機モデル
	player_.AddModel(models_[0]);
	// 3Dレティクルモデル
	player_.AddModel(models_[1]);
	// 弾モデル
	player_.AddModel(models_[2]);
	// 必要なクラスのアドレスをセットする
	player_.SetCamera(followCamera_.GetCamera());
	player_.SetGameScene(this);
	player_.SetCollisionManager(&collisionManager_);
	player_.Initialize();
	// 自キャラとレールカメラの親子関係を結ぶ
	player_.SetParent(&railCamera_.GetWorldTransform());

	// エネミーマネージャの生成
	//enemyManager_ = std::make_unique<EnemyManager>();
	// 通常敵の体
	enemyManager_.AddModel(models_[3]);
	// 通常敵の弾
	enemyManager_.Initialize();
	enemyManager_.AddModel(models_[4]);
	enemyManager_.SetCamera(followCamera_.GetCamera());
	enemyManager_.SetCollisionManager(&collisionManager_);
	enemyManager_.SetPlayer(&player_);
	enemyManager_.SetSpawnPoints(enemyPoints_);


	// 自機のロックオンクラス生成
	aimAssist_ = AimAssist::GetInstance();
	aimAssist_->SetCamera(followCamera_.GetCamera());
	aimAssist_->SetPlayer(&player_);
	player_.SetIsLockOn(aimAssist_->GetIsLockOn());
	player_.SetLockOnReticleOffset(aimAssist_->GetLockOnReticleOffset());

	// カメラの方向ベクトルをアドレスで渡す
	aimAssist_->SetCameraDirectionVelocity(railCamera_.GetDirectionVelocity());
	// レールカメラの進行度のアドレスを渡す
	enemyManager_.SetRailCameraProgress(railCamera_.GetRailPercentage());
	// アドレス渡し
	followCamera_.SetCameraOffset(player_.GetCameraOffset());
	followCamera_.SetCameraRotateOffset(player_.GetCameraRotateOffset());
	followCamera_.SetParent(&railCamera_.GetWorldTransform());
	followCamera_.SetFov(railCamera_.GetFov());
	railCamera_.SetIsBoost(player_.GetIsBoost());

	// Skybox
	//cube_ = std::make_unique<Cube>();
	//cube_->SetCamera(railCamera_->GetCamera());
	//cube_->SetScale(Vector3{ 100,100,100 });
	//cube_->SetPosition(Vector3{ 0,0,10 });

	// Blenderで読み込んだオブジェクトの設定
	for (Object3D* object : levelObjects_) {
		object->SetCamera(followCamera_.GetCamera());
		//object->SetIsLighting(true);
	}

	// UIのスプライトを作成
	//guideUI_[0] = std::make_unique<Sprite>();
	guideUI_[0].Initialize("Textures/UI", "guide_Attack.png");
	guideUI_[0].SetAnchorPoint(Vector2{ 0.5f, 0.5f });
	guideUI_[0].SetPos(Vector2{ 1200.0f, 64.0f });
	guideUI_[0].SetSize(Vector2{ 64.0f, 64.0f });
	//guideUI_[1] = std::make_unique<Sprite>();
	guideUI_[1].Initialize("Textures/UI", "guide_pad_RB.png");
	guideUI_[1].SetAnchorPoint(Vector2{ 0.5f, 0.5f });
	guideUI_[1].SetPos(Vector2{ 1132.0f, 64.0f });
	//guideUI_[2] = std::make_unique<Sprite>();
	guideUI_[2].Initialize("Textures/UI", "guide_Boost.png");
	guideUI_[2].SetAnchorPoint(Vector2{ 0.5f, 0.5f });
	guideUI_[2].SetPos(Vector2{ 1200.0f, 128.0f });
	guideUI_[2].SetSize(Vector2{ 64.0f, 64.0f });
	//guideUI_[3] = std::make_unique<Sprite>();
	guideUI_[3].Initialize("Textures/UI", "guide_pad_X.png");
	guideUI_[3].SetSize(Vector2{ 32,32 });
	guideUI_[3].SetPos(Vector2{ 1132.0f, 128.0f });
}

void GameScene::Update() {
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

	// シーン切り替え
	// クリア条件
	if (railCamera_.GetIsGameClear()) {
		sceneNum = GAMECLEAR_SCENE;
	}
	// ゲームオーバ条件
	if (player_.GetIsDead()) {
		sceneNum = GAMEOVER_SCENE;
	}

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

	// デバッグカメラの更新
	railCamera_.Update();
	followCamera_.SetPlayerPos(railCamera_.GetWorldTransform().translate);
	followCamera_.Update();

	// 衝突マネージャー(当たり判定)
	collisionManager_.CheckAllCollisions();

	audio_->SetMuffle(BGM_, 1.0f);
}

void GameScene::Draw() {
	// レールカメラの移動ルート表示
	//railCamera_.MoveRouteDraw();

	// 敵の体、弾を描画
	enemyManager_.Draw();

	// Blenderで配置したオブジェクト
	for (Object3D* object : levelObjects_) {
		object->Draw();
	}

	// 自機
	player_.Draw();
	// 自機の弾
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Draw();
	}
	// 自機のレティクルとHP
	player_.DrawUI();

	// UI
	for (int i = 0; i < 4; i++) {
		guideUI_[i].Draw();
	}

	// skybox
	//cube_->Draw(TextureManager::GetInstance()->GetSrvIndex("rostock_laage_airport_4k.dds"));
}

void GameScene::Finalize() {
	// ゲームパッドの振動を消す
	Input::GetInstance()->GamePadVibration(0, 0, 0);
	// 自機の弾
	for (PlayerBullet* bullet : playerBullets_) {
		delete bullet;
	}

	// リストのクリア
	// 自機の弾
	playerBullets_.clear();
	// 敵に関するすべて(デストラクタでやるとメンバ変数の書く場所によってバグるから解放処理を作成)

	// 基底クラスの解放
	IScene::Finalize();
}

void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	// リストに登録する
	playerBullets_.push_back(playerBullet);
}
