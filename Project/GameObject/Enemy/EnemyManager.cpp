#include "EnemyManager.h"

EnemyManager::~EnemyManager() {
	// 敵
	for (IEnemy* enemy : enemy_) {
		delete enemy;
	}
	enemy_.clear();
	// 敵の弾
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

	// 攻撃の警告
	attackAlert_ = AttackAlert::GetInstance();
	attackAlert_->Initialize();
	attackAlert_->SetCamera(camera_);
	attackAlert_->SetPlayer(player_);

	followWorldTransform_.Initialize();
	followWorldTransform_.parent_ = railCamera_->GetWorldTransform_P();
}

void EnemyManager::Update() {
	followWorldTransform_.UpdateMatrix();

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
		if (!enemy->GetIsActive()) {
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

	// 攻撃の警告
	attackAlert_->Update();
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
	particle.vel.y -= 0.3f;
}

void EnemyManager::CheckSpawn() {
	for (int i = 0; i < spawnPoints_.size(); i++) {
		if (!spawnPointDatas_[i].isActive) {
			// 指定のカメラ進行度まで達しているなら
			if ((*railCameraProgress_) >= spawnPoints_[i].percentage / 100) {
				spawnPointDatas_[i].isActive = true;
			}

			if (spawnPointDatas_[i].isActive) {
				if (spawnPoints_[i].type == "NORMAL") {
					SpawnEnemy(spawnPoints_[i].point, spawnPoints_[i].rotate, spawnPoints_[i].velocity, spawnPoints_[i].controlPoints.points);
				}
				else if (spawnPoints_[i].type == "TURRET") {
					SpawnFixedTurret(spawnPoints_[i].point, spawnPoints_[i].rotate);
				}
				else if (spawnPoints_[i].type == "BEAM") {
					SpawnBeamEnemy(spawnPoints_[i].point, spawnPoints_[i].rotate);
				}
			}
		}
	}
}

void EnemyManager::SpawnEnemy(Vector3 pos, Vector3 rotate, Vector3 moveSpeed, std::vector<Vector3> controlPoints) {
	Enemy* enemy = new Enemy();

	// 敵モデルを追加
	enemy->AddModel(models_[0]);
	// 弾のモデルを追加
	enemy->AddModel(models_[1]);
	// 必要なアドレスを設定
	enemy->SetPlayer(player_);
	enemy->SetCamera(camera_);
	enemy->SetEnemyManager(this);
	// Blenderで設定した移動速度を代入
	enemy->SetMoveSpeed(moveSpeed);
	// 移動ルート
	enemy->SetTravelRouteControlPoints(controlPoints);
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
	particle->SetEmitterCount(20);
	particle->SetEmitterSpawnCount(1);
	particle->randomScaleLimit = { 0.3f, 0.4f };
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
	particle->SetEmitterCount(20);
	particle->SetEmitterSpawnCount(1);
	particle->randomScaleLimit = { 0.3f, 0.4f };
	spawnParticles_.push_back(particle);
}

void EnemyManager::SpawnBeamEnemy(Vector3 pos, Vector3 rotate) {
	BeamEnemy* enemy = new BeamEnemy();

	// 敵モデルを追加
	enemy->AddModel(models_[0]);
	// ビームのモデルを追加
	enemy->AddModel(models_[1]);
	// 必要なアドレスを設定
	enemy->SetPlayer(player_);
	enemy->SetCamera(camera_);
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
	particle->SetEmitterCount(20);
	particle->SetEmitterSpawnCount(1);
	particle->randomScaleLimit = { 0.3f, 0.4f };
	spawnParticles_.push_back(particle);
}

void EnemyManager::SpawnTitan(Vector3 pos, Vector3 rotate) {
	Titan* boss = new Titan();

	// 敵モデルを追加
	boss->AddModel(models_[0]);
	// 必要なアドレスを設定
	boss->SetPlayer(player_);
	boss->SetCamera(camera_);
	boss->SetEnemyManager(this);
	boss->SetParent(&followWorldTransform_);
	// 初期化
	// カメラの座標が中心なのでspawnPosはカメラとの距離
	boss->Initialize(pos, rotate, id_);
	// リストに登録
	enemy_.push_back(boss);

	// 管理番号更新
	id_++;

	// 出現時のパーティクルを生成
	Particles* particle = new Particles();
	particle->Initialize(boss->GetWorldPosition() + pos);
	particle->SetCamera(camera_);
	particle->SetEmitterFrequency(1);
	particle->SetEmitterCount(20);
	particle->SetEmitterSpawnCount(1);
	particle->randomScaleLimit = { 0.3f, 0.4f };
	spawnParticles_.push_back(particle);
}

bool EnemyManager::IsObjectInOppositeDirection(const Vector3& objectPosition) {
	// カメラの角度方向ベクトルに変換
	Vector3 offset{ 0, 0, 1 };
	// 回転行列を合成
	Matrix4x4 rotateMatrix = MakeRotateMatrix(camera_->worldTransform_.parent_->rotate);
	// 自機のワールド行列の回転を反映する
	offset = TransformNormal(offset, rotateMatrix);

	// 自機と敵の方向ベクトルを算出
	Vector3 p2eDirVel = Normalize(objectPosition - camera_->worldTransform_.translate);

	float dotXZ = Dot(Vector2{ offset.x,offset.z }, Vector2{ p2eDirVel.x,p2eDirVel.z });
	float magnitude1XZ = Length(Vector2{ offset.x,offset.z });
	float magnitude2XZ = Length(Vector2{ p2eDirVel.x,p2eDirVel.z });
	float angleXZ = std::acos(dotXZ / (magnitude1XZ * magnitude2XZ));
	angleXZ = Radian2Degree(angleXZ);

	if ((angleXZ) < (90.0f)) {
		return false;
	}
	// カメラの映らないところにいる
	return true;
}

void EnemyManager::CheckActiveState() {
	for (IEnemy* enemy : enemy_) {
		// カメラの後ろなら敵の描画と機能停止
		Vector2 enemyPos = ConvertWorld2Screen(enemy->GetWorldPosition());
		if (enemyPos.x >= -50.0f && enemyPos.x <= 1330.0f &&
			enemyPos.y >= -50.0f && enemyPos.y <= 770.0f) {		
			if (IsObjectInOppositeDirection(enemy->GetWorldPosition())) {
				//enemy->SetIsActive(false);
			}
		}
		else {
			if (IsObjectInOppositeDirection(enemy->GetWorldPosition())) {
				//enemy->SetIsActive(false);
			}
		}
	}
}

Vector2 EnemyManager::ConvertWorld2Screen(Vector3 worldPos) {
	Vector3 result = worldPos + 0.00001f;

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

void EnemyManager::AppearanceDirectionEditor() {

}