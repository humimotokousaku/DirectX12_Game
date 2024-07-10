#include "EnemyManager.h"

EnemyManager::EnemyManager() {
}
EnemyManager::~EnemyManager() {
	Finalize();
}

void EnemyManager::Initialize() {
	// 敵の出現情報を読み込む
	LoadEnemyPopData();
}

void EnemyManager::Update() {
	// 敵の出現するタイミングと座標
	//UpdateEnemyPopCommands();
	CheckSpawn();

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
		//enemy->Update();
	}
	// 弾の更新
	for (EnemyBullet* bullet : enemyBullets_) {
		//bullet->Update();
	}
	// 終了した弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->isDead()) {
			delete bullet;
			return true;
		}
		return false;
		});
}

void EnemyManager::Draw() {
	// 通常敵
	for (Enemy* enemy : enemy_) {
		enemy->Draw();
	}
	// 通常敵の弾
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw();
	}
}

void EnemyManager::Finalize() {
	// 通常敵
	for (Enemy* enemy : enemy_) {
		delete enemy;
	}
	enemy_.clear();
	// 通常敵の弾
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
	enemyBullets_.clear();
	models_.clear();
	enemyPopCommands_.clear();
}

void EnemyManager::CheckSpawn() {
	for (int i = 0; i < spawnPoints_.size(); i++) {
		if (!spawnPointDatas_[i].isActive) {
			if (railCameraProgress_ >= spawnPoints_[i].percentage / 100) {
				spawnPointDatas_[i].isActive = true;
			}

			if (spawnPointDatas_[i].isActive) {
				SpawnEnemy(spawnPoints_[i].point);
			}
		}
	}
}

void EnemyManager::SpawnEnemy(Vector3 pos) {
	Enemy* enemy = new Enemy();

	// 敵モデルを追加
	enemy->AddModel(models_[0]);
	// 弾のモデルを追加
	enemy->AddModel(models_[1]);

	// 必要なアドレスを設定
	enemy->SetPlayer(player_);
	enemy->SetCamera(camera_);
	enemy->SetCollisionManager(collisionManager_);
	enemy->SetEnemyManager(this);

	// 初期化
	enemy->Initialize(pos);

	// リストに登録
	enemy_.push_back(enemy);
}

void EnemyManager::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("Engine/resources/csv/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands_ << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void EnemyManager::UpdateEnemyPopCommands() {
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