#include "EnemyManager.h"

EnemyManager::~EnemyManager() {
	// 通常敵
	for (IEnemy* enemy : enemy_) {
		delete enemy;
	}
	enemy_.clear();
	// 通常敵の弾
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
	enemyBullets_.clear();

	// 出現時のパーティクル
	for (Particles* particle : spawnParticles_) {
		delete particle;
	}
	spawnParticles_.clear();
	// 被弾時のパーティクル
	for (Particles* particle : hitParticles_) {
		delete particle;
	}
	hitParticles_.clear();

	models_.clear();
}

void EnemyManager::Initialize() {
	textureManager_ = TextureManager::GetInstance();
	audio_ = Audio::GetInstance();
	// スコアのインスタンスを取得
	score_ = Score::GetInstance();

	// テクスチャの読み込み
	textureManager_->LoadTexture("DefaultTexture", "white.png");
	textureManager_->LoadTexture("Textures", "hitParticle.png");
	spawnParticleTex_ = textureManager_->GetSrvIndex("DefaultTexture", "white.png");
	hitParticleTex_ = textureManager_->GetSrvIndex("Textures", "hitParticle.png");

	// 死亡SEの読み込み
	deadSE_ = audio_->SoundLoadWave("Audio/dead.wav");
}

void EnemyManager::Update() {
	// 出現条件を満たしているかを確認
	CheckSpawn();

	// カメラの後ろにいるなら機能停止
	CheckActiveState();

	// 敵の削除
	enemy_.remove_if([&](IEnemy* enemy) {
		if (enemy->IsDead()) {
			// 割り振られたスコアを加算
			score_->AddScore(enemy->GetScore());
			// 死亡SEを再生
			audio_->SoundPlayWave(deadSE_, false, 0.3f);
			delete enemy;
			return true;
		}
		return false;
		});
	// enemyの更新
	for (IEnemy* enemy : enemy_) {
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

	// 出現時のパーティクル
	for (Particles* particle : spawnParticles_) {
		particle->Update();
	}
	// 被弾時のパーティクル
	for (Particles* hitParticle : hitParticles_) {
		hitParticle->Update();
	}
}

void EnemyManager::Draw() {
	// 敵
	for (IEnemy* enemy : enemy_) {
		enemy->Draw();
	}
	// 通常敵の弾
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw();
	}
}

void EnemyManager::DrawParticle() {
	// 出現時のパーティクル
	for (Particles* particle : spawnParticles_) {
		particle->Draw(spawnParticleTex_);
	}
	// 被弾時のパーティクル
	for (Particles* hitParticle : hitParticles_) {
		hitParticle->Draw(hitParticleTex_);
	}
}

void EnemyManager::ParticleUpdate(Particle& particle) {
	particle.vel.x *= 0.9f;
	particle.vel.z *= 0.9f;
	particle.vel.y -= 0.1f;
}

void EnemyManager::CheckSpawn() {
	for (int i = 0; i < spawnPoints_.size(); i++) {
		if (!spawnPointDatas_[i].isActive) {
			if ((*railCameraProgress_) >= spawnPoints_[i].percentage / 100) {
				//if (i == 0) {
					spawnPointDatas_[i].isActive = true;
				//}
			}

			if (spawnPointDatas_[i].isActive) {
				if (spawnPoints_[i].type == "normal") {
					SpawnEnemy(spawnPoints_[i].point, spawnPoints_[i].rotate, spawnPoints_[i].velocity);
				}
				else if (spawnPoints_[i].type == "turret") {
					SpawnFixedTurret(spawnPoints_[i].point, spawnPoints_[i].rotate);
				}
			}
		}
	}
}

void EnemyManager::SpawnEnemy(Vector3 pos, Vector3 rotate, Vector3 moveSpeed) {
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
	// Blenderで設定した移動速度を代入
	enemy->SetMoveSpeed(moveSpeed);
	// 初期化
	enemy->Initialize(pos, rotate, id_);
	// リストに登録
	enemy_.push_back(enemy);

	// 管理番号更新
	id_++;

	// 出現時のパーティクルを生成
	Particles* particle = new Particles();
	particle->Initialize(pos);
	particle->SetCamera(camera_);
	particle->SetEmitterFrequency(1);
	particle->SetEmitterCount(40);
	particle->SetEmitterSpawnCount(1);
	particle->randomScaleLimit = { 0.3f, 0.4f };
	particle->SetParticleUpdate(std::bind(&EnemyManager::ParticleUpdate, this, std::placeholders::_1));
	spawnParticles_.push_back(particle);
}

void EnemyManager::SpawnFixedTurret(Vector3 pos, Vector3 rotate) {
	FixedTurret* enemy = new FixedTurret();

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
	enemy->Initialize(pos, rotate, id_);
	// リストに登録
	enemy_.push_back(enemy);

	// 管理番号更新
	id_++;

	// 出現時のパーティクルを生成
	Particles* particle = new Particles();
	particle->Initialize(pos);
	particle->SetCamera(camera_);
	particle->SetEmitterFrequency(1);
	particle->SetEmitterCount(40);
	particle->SetEmitterSpawnCount(1);
	particle->randomScaleLimit = { 0.3f, 0.4f };
	particle->SetParticleUpdate(std::bind(&EnemyManager::ParticleUpdate, this, std::placeholders::_1));
	spawnParticles_.push_back(particle);
}

bool EnemyManager::IsObjectInOppositeDirection(const Vector3& direction, const Vector3& objectPosition) {
	float dot = Dot(Normalize(direction), Normalize(objectPosition));
	float angle = std::acos(dot); // ラジアンで返す

	// -π/2 ~ π/2の範囲に含まれていないかを確認
	if (std::fabs(angle) < M_PI / 2) {
		return true;
	}
	return false;
}

void EnemyManager::CheckActiveState() {
	for (IEnemy* enemy : enemy_) {
		// カメラの後ろなら敵の描画と機能停止
		Vector2 enemyPos = ConvertWorld2Screen(enemy->GetWorldPosition());
		if (enemyPos.x >= 0.0f && enemyPos.x <= 1280.0f &&
			enemyPos.y >= 0.0f && enemyPos.y <= 720.0f) {
			if (!IsObjectInOppositeDirection(*cameraMoveVel_, enemy->GetWorldPosition())) {
				enemy->SetIsActive(false);
			}
		}
		else {
			enemy->SetIsActive(false);
		}
	}
}

Vector2 EnemyManager::ConvertWorld2Screen(Vector3 worldPos) {
	Vector3 result = worldPos;
	if (result.x == 0 && result.y == 0 && result.z == 0) {
		result = { 0.000001f,0.000001f ,0.000001f };
	}
	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport{};
	matViewProjectionViewport =
		Multiply(camera_->GetViewProjection().matView, Multiply(camera_->GetViewProjection().matProjection, matViewport));
	// ワールド→スクリーン座標変換
	result = Transforms(result, matViewProjectionViewport);

	return Vector2(result.x, result.y);
}
