#include "GameScene.h"
#include "GameManager.h"

GameScene::GameScene() {

}

void GameScene::Initialize() {
	sceneNum = GAME_SCENE;

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

	// 敵の出現情報を読み込む
	LoadEnemyPopData();

	// カメラの生成
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	// カメラレールの生成
	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Initialize();

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

	// Blender
	LoadJSONFile("sample_map.json");
	for (Object3D* object : levelObjects_) {
		object->SetCamera(railCamera_->GetCamera());
		object->SetIsLighting(false);

	}
	// 線
	for (int i = 0; i < segmentCount; i++) {
		line_[i] = std::make_unique<Line>();
		line_[i]->Initialize();
		line_[i]->SetCamera(railCamera_->GetCamera());
	}

	// 線分の数+1個分の頂点座標の計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		if (t >= 1.0f) {
			t = 0.99f;
		}
		Vector3 pos = Lerps::CatmullRomSpline(controlPoints_, t);
		// 描画用頂点リストに追加
		pointsDrawing_.push_back(pos);
	}
	for (int i = 0; i < segmentCount; i++) {
		line_[i]->startPos_ = pointsDrawing_[i];
		line_[i]->endPos_ = pointsDrawing_[i + 1];
	}
	// スプライン曲線制御点（通過点）の初期化
	/*controlPoints_ = {
		{0,  0,  0},
		{0,  0,  30},
		{10, 10, 10},
		{10, 15, 0},
		{20, 15, 20},
		{20, 0,  0},
		{30, 0,  -10}
	};*/
	t_ = 0.0f;
	targetT_ = 1.0f / segmentCount;
	//isMoveCamera_ = true;


	// Skybox
	cube_ = std::make_unique<Cube>();
	cube_->SetCamera(railCamera_->GetCamera());
	cube_->SetScale(Vector3{ 100,100,100 });
	cube_->SetPosition(Vector3{ 0,0,10 });
}

void GameScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		sceneNum = TITLE_SCENE;
	}

	// 敵の出現するタイミングと座標
	UpdateEnemyPopCommands();
	// 敵の削除
	enemy_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
	// enemyの更新
	for (Enemy* enemy : enemy_) {
		enemy->Update();
	}
	// 弾の更新
	for (EnemyBullet* bullet : enemyBullets_) {
  		bullet->Update();
	}
	// 終了した弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->isDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

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

	if (isMoveCamera_) {
		// カメラの移動
		if (t_ <= 1.0f) {
			t_ += 1.0f / segmentCount / 1;		
		}	
		// 
		if(targetT_ <= 1.0f){
			targetT_ += 1.0f / segmentCount / 1;			
		}
		if (targetT_ >= 1.0f) {
			targetT_ = 0.999f;
			t_ = 0.98f;
			isMoveCamera_ = false;
		}
			
	}
	target_ = Lerps::CatmullRomSpline(controlPoints_, targetT_);
	UpdatePlayerPosition(t_);
	// デバッグカメラの更新
	railCamera_->Update(target_);

	// 衝突マネージャー(当たり判定)
	collisionManager_->CheckAllCollisions();

///#ifdef USE_IMGUI

	ImGui::Begin("Current Scene");
	ImGui::Text("GAME");
	ImGui::Text("SPACE:scene change");
	ImGui::DragFloat("cameraT", &targetT_, 0);
	ImGui::DragFloat("playerT", &t_, 0);
	ImGui::End();
//#endif
}

void GameScene::Draw() {
	for (int i = 0; i < segmentCount; i++) {
		line_[i]->Draw();
	}

	// 自機
	player_->Draw();
	// 自機の弾
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Draw();
	}

	// 通常敵
	for (Enemy* enemy : enemy_) {
		enemy->Draw();
	}
	// 通常敵の弾
	for (EnemyBullet* bullet : enemyBullets_) {
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
	// 通常敵
	for (Enemy* enemy : enemy_) {
		delete enemy;
	}
	// 通常敵の弾
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}

	// リストのクリア
	// 自機の弾
	playerBullets_.clear();
	// 通常敵
	enemy_.clear();
	// 通常敵の弾
	enemyBullets_.clear();
}

void GameScene::UpdatePlayerPosition(float t) {
	Vector3 cameraPosition{};
	// Catmull-Romスプライン関数で補間された位置を取得
	cameraPosition = Lerps::CatmullRomSpline(controlPoints_, t);
	cameraPosition.y += 0.02f;
	//cameraPosition.z += 0.01f;
	railCamera_->SetTranslation(cameraPosition);
}

void GameScene::SpawnEnemy(Vector3 pos) {
	Enemy* enemy = new Enemy();

	// 敵モデルを追加
	enemy->AddModel(models_[3]);
	// 弾のモデルを追加
	enemy->AddModel(models_[4]);

	// 必要なアドレスを設定
	enemy->SetPlayer(player_.get());
	enemy->SetCamera(railCamera_->GetCamera());
	enemy->SetCollisionManager(collisionManager_.get());
	enemy->SetGameScene(this);

	// 初期化
	enemy->Initialize(pos);

	// リストに登録
	enemy_.push_back(enemy);
}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("Engine/resources/csv/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands_ << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {
	// 待機処理
	if (isWait_) {
		waitTime_--;
		if (waitTime_ <= 0) {
			// 待機完了
			isWait_ = false;
		}
		return;
	}

	// 1桁分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands_, line)) {
		// 1桁の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			SpawnEnemy(Vector3(x, y, z));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待ち時間
			isWait_ = true;
			waitTime_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	// リストに登録する
	playerBullets_.push_back(playerBullet);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}