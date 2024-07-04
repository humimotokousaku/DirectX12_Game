#include "GameScene.h"
#include "GameManager.h"

GameScene::GameScene() {

}

void GameScene::Initialize() {
	sceneNum = GAME_SCENE;

#pragma region 読み込み
	TextureManager::GetInstance()->LoadTexture("", "reticle.png");
	// 使用するモデルの読み込み
	ModelManager::GetInstance()->LoadModel("", "block.obj");

	// 自機
	AddModel(ModelManager::GetInstance()->SetModel("", "block.obj"));
	// 3Dレティクル
	AddModel(ModelManager::GetInstance()->SetModel("", "block.obj"));
	// 自機の弾
	AddModel(ModelManager::GetInstance()->SetModel("", "block.obj"));
	// 通常敵のモデル
	AddModel(ModelManager::GetInstance()->SetModel("", "block.obj"));
	// 通常敵の弾
	AddModel(ModelManager::GetInstance()->SetModel("", "block.obj"));

	// Blender
	LoadJSONFile("sample_map.json");
#pragma endregion

	// カメラの生成
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	// カメラレールの生成
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize(controlPoints_);

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
	player_->SetCamera(railCamera_->GetCamera());
	player_->SetGameScene(this);
	player_->SetCollisionManager(collisionManager_.get());
	player_->Initialize();
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	// エネミーマネージャの生成
	enemyManager_ = std::make_unique<EnemyManager>();
	//enemyManager_ = new EnemyManager();
	// 通常敵の体
	enemyManager_->AddModel(models_[3]);
	// 通常敵の弾
	enemyManager_->AddModel(models_[4]);
	enemyManager_->SetCamera(railCamera_->GetCamera());
	enemyManager_->SetCollisionManager(collisionManager_.get());
	enemyManager_->SetPlayer(player_.get());
	enemyManager_->Initialize();

	// Skybox
	cube_ = std::make_unique<Cube>();
	cube_->SetCamera(railCamera_->GetCamera());
	cube_->SetScale(Vector3{ 100,100,100 });
	cube_->SetPosition(Vector3{ 0,0,10 });

	// Blenderで読み込んだオブジェクトの設定
	for (Object3D* object : levelObjects_) {
		object->SetCamera(railCamera_->GetCamera());
		//object->SetIsLighting(false);
	}
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

	// 衝突マネージャー(当たり判定)
	collisionManager_->CheckAllCollisions();
}

void GameScene::Draw() {
	// レールカメラの移動ルート表示
	railCamera_->MoveRouteDraw();

	enemyManager_->Draw();

	// 自機
	player_->Draw();
	// 自機の弾
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Draw();
	}

	// Blenderで配置したオブジェクト
	for (Object3D* object : levelObjects_) {
		object->Draw();
	}

	// 自機のレティクル
	player_->DrawUI();

	// skybox
	cube_->Draw(TextureManager::GetInstance()->GetSrvIndex("rostock_laage_airport_4k.dds"));
}

void GameScene::Finalize() {
	// 自機の弾
	for (PlayerBullet* bullet : playerBullets_) {
		delete bullet;
	}

	//delete enemyManager_;
	enemyManager_->Finalize();
	collisionManager_->ClearColliderList();

	// リストのクリア
	// 自機の弾
	playerBullets_.clear();

	// 基底クラスの解放
	IScene::Finalize();
}

void GameScene::UpdatePlayerPosition(float t) {
	Vector3 cameraPosition{};
	// Catmull-Romスプライン関数で補間された位置を取得
	cameraPosition = Lerps::CatmullRomSpline(controlPoints_, t);
	railCamera_->SetTranslation(cameraPosition);
}

void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	// リストに登録する
	playerBullets_.push_back(playerBullet);
}
