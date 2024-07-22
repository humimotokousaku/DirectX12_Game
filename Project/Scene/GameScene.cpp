#include "GameScene.h"
#include "GameManager.h"

GameScene::GameScene() {

}

void GameScene::Initialize() {
	sceneNum = GAME_SCENE;
	textureManager_ = TextureManager::GetInstance();
	modelManager_ = ModelManager::GetInstance();

#pragma region 読み込み
	textureManager_->LoadTexture("", "reticle.png");
	textureManager_->LoadTexture("Textures", "Bob_Red.png");
	textureManager_->LoadTexture("Textures", "Spitfire_Purple.png");
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
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	// カメラレールの生成
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize(controlPoints_);
	// 追従カメラの生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();

	// 衝突マネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	// 自キャラの生成
	player_ = std::make_unique<Player>();
	// 自機モデル
	player_->AddModel(models_[0]);
	// 3Dレティクルモデル
	player_->AddModel(models_[1]);
	// 弾モデル
	player_->AddModel(models_[2]);
	// 必要なクラスのアドレスをセットする
	player_->SetCamera(followCamera_->GetCamera());
	player_->SetGameScene(this);
	player_->SetCollisionManager(collisionManager_.get());
	player_->Initialize();
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	// エネミーマネージャの生成
	enemyManager_ = std::make_unique<EnemyManager>();
	// 通常敵の体
	enemyManager_->AddModel(models_[3]);
	// 通常敵の弾
	enemyManager_->AddModel(models_[4]);
	enemyManager_->SetCamera(followCamera_->GetCamera());
	enemyManager_->SetCollisionManager(collisionManager_.get());
	enemyManager_->SetPlayer(player_.get());
	enemyManager_->SetSpawnPoints(enemyPoints_);
	enemyManager_->Initialize();

	// 自機のロックオンクラス生成
	aimAssist_ = AimAssist::GetInstance();
	aimAssist_->SetCamera(followCamera_->GetCamera());
	aimAssist_->SetPlayer(player_.get());

	// カメラの方向ベクトルをアドレスで渡す
	aimAssist_->SetCameraDirectionVelocity(railCamera_->GetDirectionVelocity());
	// レールカメラの進行度のアドレスを渡す
	enemyManager_->SetRailCameraProgress(railCamera_->GetRailPercentage());
	// 
	followCamera_->SetCameraOffset(player_->GetCameraOffset());
	followCamera_->SetCameraRotateOffset(player_->GetCameraRotateOffset());
	followCamera_->SetParent(&railCamera_->GetWorldTransform());

	// Skybox
	//cube_ = std::make_unique<Cube>();
	//cube_->SetCamera(railCamera_->GetCamera());
	//cube_->SetScale(Vector3{ 100,100,100 });
	//cube_->SetPosition(Vector3{ 0,0,10 });

	// Blenderで読み込んだオブジェクトの設定
	for (Object3D* object : levelObjects_) {
		object->SetCamera(followCamera_->GetCamera());
		//object->SetIsLighting(true);
	}

	// UIのスプライトを作成
	guideUI_[0] = std::make_unique<Sprite>();
	guideUI_[0]->Initialize("Textures/UI", "guide_Attack.png");
	guideUI_[0]->SetAnchorPoint(Vector2{ 0.5f, 0.5f });
	guideUI_[0]->SetPos(Vector2{ 1200.0f, 64.0f });
	guideUI_[0]->SetSize(Vector2{ 64.0f, 64.0f });
	guideUI_[1] = std::make_unique<Sprite>();
	guideUI_[1]->Initialize("Textures/UI", "guide_pad_RB.png");
	guideUI_[1]->SetAnchorPoint(Vector2{ 0.5f, 0.5f });
	guideUI_[1]->SetPos(Vector2{ 1132.0f, 64.0f });
}

void GameScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		sceneNum = TITLE_SCENE;
	}
	if (Input::GetInstance()->TriggerKey(DIK_2)) {
		sceneNum = GAMEOVER_SCENE;
	}
	if (Input::GetInstance()->TriggerKey(DIK_3)) {
		sceneNum = GAMECLEAR_SCENE;
	}


	enemyManager_->Update();
	aimAssist_->SetEnemyList(enemyManager_->GetEnemyList());

	// 自キャラの更新
	player_->Update();
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
	railCamera_->Update();
	//followCamera_->SetPlayerPos(player_->GetWorldPosition());
	followCamera_->SetPlayerPos(railCamera_->GetWorldTransform().translate);
	followCamera_->Update();

	// 衝突マネージャー(当たり判定)
	collisionManager_->CheckAllCollisions();
}

void GameScene::Draw() {
	// レールカメラの移動ルート表示
	//railCamera_->MoveRouteDraw();

	// 敵の体、弾を描画
	enemyManager_->Draw();

	// Blenderで配置したオブジェクト
	for (Object3D* object : levelObjects_) {
		object->Draw();
	}

	// 自機
	player_->Draw();
	// 自機の弾
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Draw();
	}
	// 自機のレティクルとHP
	player_->DrawUI();

	// UI
	for (int i = 0; i < 2; i++) {
		guideUI_[i]->Draw();
	}

	// skybox
	//cube_->Draw(TextureManager::GetInstance()->GetSrvIndex("rostock_laage_airport_4k.dds"));
}

void GameScene::Finalize() {
	// 自機の弾
	for (PlayerBullet* bullet : playerBullets_) {
		delete bullet;
	}

	// リストのクリア
	// 自機の弾
	playerBullets_.clear();

	// 基底クラスの解放
	IScene::Finalize();
}

void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	// リストに登録する
	playerBullets_.push_back(playerBullet);
}
