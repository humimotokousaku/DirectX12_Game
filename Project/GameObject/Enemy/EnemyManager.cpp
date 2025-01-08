#include "EnemyManager.h"

EnemyManager::~EnemyManager() {
	// 敵
	for (BaseEnemy* enemy : enemys_) {
		delete enemy;
	}
	enemys_.clear();
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
	// 死亡時のパーティクル
	for (DeadParicles* deadParicles : deadParicles_) {
		delete deadParicles;
	}
	spawnParticles_.clear();
	hitParticles_.clear();
	deadParicles_.clear();

	models_.clear();
}

void EnemyManager::Initialize() {
	textureManager_ = TextureManager::GetInstance();
	audio_ = Audio::GetInstance();
	// スコアのインスタンスを取得
	score_ = Score::GetInstance();

	// テクスチャの読み込み
	textureManager_->LoadTexture("Textures/DefaultTexture", "white.png");
	textureManager_->LoadTexture("Textures", "hitParticle.png");
	// 死亡パーティクル
	for (int i = 1; i <= kAnimationNum; i++) {
		std::string num = std::to_string(i).c_str();
		textureManager_->LoadTexture("Textures", "deadParticle" + num + ".png");
	}
	spawnParticleTextures_.particle = textureManager_->GetSrvIndex("Textures", "deadParticle4.png");
	spawnParticleTextures_.dissolve = textureManager_->GetSrvIndex("Textures", "noise.png");
	hitParticleTextures_.particle = textureManager_->GetSrvIndex("Textures", "deadParticle4.png");
	hitParticleTextures_.dissolve = textureManager_->GetSrvIndex("Textures", "noise.png");
	bulletTexture_ = textureManager_->GetSrvIndex("Textures/DefaultTexture", "white.png");
	// 死亡SEの読み込み
	deadSE_ = audio_->SoundLoadWave("Audio/dead.wav");

	// テクスチャ読み込み
	for (int i = 1; i <= kAnimationNum / kAnimationSpeed; i++) {
		std::string num = std::to_string(i).c_str();
		for (int animSpeed = 0; animSpeed < kAnimationSpeed; animSpeed++) {
			deadParticleTextures_.push_back(textureManager_->GetSrvIndex("Textures", "deadParticle" + num + ".png"));
		}
	}

	// 爆発パーティクル用のビルボードカメラ
	billboardCamera_ = std::make_unique<Camera>();
	billboardCamera_->Initialize();
	billboardCamera_->viewProjection_.matProjection = MakeIdentity4x4();
	billboardCamera_->viewProjection_.matView = MakeIdentity4x4();
	billboardCamera_->TransferMatrix();

	for (int i = 0; i < spawnPoints_.size(); i++) {
		// 死亡パーティクルを生成
		CreateDeadParticle(Vector3{ 0,0,0 });
	}
}

void EnemyManager::Update() {
	// 出現条件を満たしているかを確認
	CheckSpawn();

	// カメラの後ろにいるなら機能停止
	CheckActiveState();

	// 敵の削除
	enemys_.remove_if([&](BaseEnemy* enemy) {
		// 倒したら点数を加算して解放
		if (enemy->IsDead()) {
			// 死亡時のパーティクルを発生
			for (DeadParicles* deadParicles : deadParicles_) {
				if (deadParicles->GetId() == -1) {
					deadParicles->SetId(enemy->GetId());
					deadParicles->CreateParticle(enemy->GetWorldPosition());
					break;
				}
			}

			// 管理番号リストから削除
			idList_.erase(std::remove(idList_.begin(), idList_.end(), enemy->GetId()), idList_.end());
			// 割り振られたスコアを加算
			score_->AddScore(enemy->GetScore());
			// 死亡SEを再生
			audio_->SoundPlayWave(deadSE_, false, 0.3f);
			delete enemy;
			return true;
		}
		// 機能停止状態なら解放
		if (!enemy->GetIsActive()) {
			// 管理番号リストから削除
			idList_.erase(std::remove(idList_.begin(), idList_.end(), enemy->GetId()), idList_.end());
			delete enemy;
			return true;
		}
		return false;
		});
	// enemyの更新
	for (BaseEnemy* enemy : enemys_) {
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

	// 死亡時のパーティクル
	for (DeadParicles* deadParicles : deadParicles_) {
		deadParicles->Update();
	}
}

void EnemyManager::Draw() {
	// 敵
	for (BaseEnemy* enemy : enemys_) {
		enemy->Draw();
	}
	// 通常敵の弾
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(bulletTexture_);
	}
}

void EnemyManager::DrawParticle() {
	// 死亡時のパーティクル
	for (DeadParicles* deadParicles : deadParicles_) {
		deadParicles->Draw(billboardCamera_->viewProjection_);
	}
}

void EnemyManager::CreateDeadParticle(const Vector3& enemyPos) {
	DeadParicles* deadParicles = new DeadParicles();
	deadParicles->SetDeadParticleTextures(deadParticleTextures_);
	deadParicles->Initialize(camera_, enemyPos);

	deadParicles_.push_back(deadParicles);
}

void EnemyManager::CheckSpawn() {
	for (LevelData::EnemyPoint& spawnPoint : spawnPoints_) {
		// すでに出現している
		if (spawnPoint.isActive) { continue; }
		// 指定のカメラ進行度まで達していない
		if ((railCamera_->GetRailPercentage()) <= spawnPoint.percentage / 100.0f) { continue; }

		// 通常の敵
		if (spawnPoint.type == "NORMAL") {
			SpawnEnemy(spawnPoint.point, spawnPoint.rotate, spawnPoint.velocity, spawnPoint.controlPoints.points);
		}
		// 弾を撃つ敵
		else if (spawnPoint.type == "TURRET") {
			SpawnFixedTurret(spawnPoint.point, spawnPoint.rotate, spawnPoint.controlPoints.points);
		}
		// ビームを撃つ敵
		else if (spawnPoint.type == "BEAM") {
			SpawnBeamEnemy(spawnPoint.point, spawnPoint.rotate, spawnPoint.controlPoints.points);
		}
		// 出現済みにする
		spawnPoint.isActive = true;
	}
}

void EnemyManager::SpawnEnemy(Vector3 pos, Vector3 rotate, Vector3 moveSpeed, std::vector<Vector3> controlPoints) {
	MoveEnemy* enemy = new MoveEnemy();

	// 敵モデルを追加
	enemy->AddModel(models_["EnemyBody"]);
	// 弾のモデルを追加
	enemy->AddModel(models_["EnemyBullet"]);
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
	enemys_.push_back(enemy);

	// 出現時のパーティクルを生成
	Particles* particle = new Particles();
	particle->Initialize(pos);
	particle->SetCamera(camera_);
	particle->SetTextures(spawnParticleTextures_);
	particle->SetIsDissolve(false);
	particle->SetEmitterFrequency(1);
	particle->SetEmitterCount(20);
	particle->SetEmitterSpawnCount(1);
	particle->randomScaleLimit = { 0.3f, 0.4f };
	spawnParticles_.push_back(particle);

	// 管理番号リストに登録
	idList_.push_back(id_);

	// 管理番号更新
	id_++;
}

void EnemyManager::SpawnFixedTurret(Vector3 pos, Vector3 rotate, std::vector<Vector3> controlPoints) {
	FixedTurret* enemy = new FixedTurret();

	// 敵モデルを追加
	enemy->AddModel(models_["EnemyBody"]);
	// 弾のモデルを追加
	enemy->AddModel(models_["EnemyBullet"]);
	// 必要なアドレスを設定
	enemy->SetPlayer(player_);
	enemy->SetCamera(camera_);
	enemy->SetEnemyManager(this);
	// 移動ルート
	enemy->SetTravelRouteControlPoints(controlPoints);

	enemy->SetParent(&railCamera_->GetWorldTransform());
	// 初期化
	enemy->Initialize(pos, rotate, id_);

	// リストに登録
	enemys_.push_back(enemy);

	// 管理番号リストに登録
	idList_.push_back(id_);

	// 管理番号更新
	id_++;

	// 出現時のパーティクルを生成
	Particles* particle = new Particles();
	particle->Initialize(pos);
	particle->SetCamera(camera_);
	particle->SetTextures(spawnParticleTextures_);
	particle->SetIsDissolve(false);
	particle->SetEmitterFrequency(1);
	particle->SetEmitterCount(20);
	particle->SetEmitterSpawnCount(1);
	particle->randomScaleLimit = { 0.3f, 0.4f };
	spawnParticles_.push_back(particle);
}

void EnemyManager::SpawnBeamEnemy(Vector3 pos, Vector3 rotate, std::vector<Vector3> controlPoints) {
	BeamEnemy* enemy = new BeamEnemy();

	// 敵モデルを追加
	enemy->AddModel(models_["EnemyBody"]);
	// ビームのモデルを追加
	enemy->AddModel(models_["EnemyBeam"]);
	// 必要なアドレスを設定
	enemy->SetPlayer(player_);
	enemy->SetCamera(camera_);
	enemy->SetEnemyManager(this);
	// 移動ルート
	enemy->SetTravelRouteControlPoints(controlPoints);
	// 初期化
	enemy->Initialize(pos, rotate, id_);
	// リストに登録
	enemys_.push_back(enemy);

	// 管理番号リストに登録
	idList_.push_back(id_);

	// 管理番号更新
	id_++;

	// 出現時のパーティクルを生成
	Particles* particle = new Particles();
	particle->Initialize(pos);
	particle->SetCamera(camera_);
	particle->SetTextures(spawnParticleTextures_);
	particle->SetIsDissolve(false);
	particle->SetEmitterFrequency(1);
	particle->SetEmitterCount(20);
	particle->SetEmitterSpawnCount(1);
	particle->randomScaleLimit = { 0.3f, 0.4f };
	spawnParticles_.push_back(particle);
}

void EnemyManager::CheckActiveState() {
	for (BaseEnemy* enemy : enemys_) {
		// カメラの正面に敵がいるなら処理しない
		if (!IsObjectInOppositeDirection(enemy->GetWorldPosition(), railCamera_->GetWorldTransform().worldPos, railCamera_->GetDirectionVelocity())) { continue; }
		// 機能停止可能でないなら処理しない
		if (!enemy->GetIsStopActive()) { continue; }
		enemy->SetIsActive(false);
	}

	for (EnemyBullet* bullet : enemyBullets_) {
		// カメラの後ろなら敵の描画と機能停止
		if (!IsObjectInOppositeDirection(bullet->GetWorldPosition(), railCamera_->GetWorldTransform().worldPos, railCamera_->GetDirectionVelocity())) { continue; }
		bullet->SetIsDead(true);
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