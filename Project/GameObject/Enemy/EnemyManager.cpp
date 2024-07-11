#include "EnemyManager.h"

EnemyManager::EnemyManager() {
}
EnemyManager::~EnemyManager() {
	Finalize();
}

void EnemyManager::Initialize() {

}

void EnemyManager::Update() {
	// 出現条件を満たしているかを確認
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
				SpawnEnemy(spawnPoints_[i].point, spawnPoints_[i].rotate);
			}
		}
	}
}

void EnemyManager::SpawnEnemy(Vector3 pos, Vector3 rotate) {
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
	enemy->Initialize(pos, rotate);

	// リストに登録
	enemy_.push_back(enemy);
}