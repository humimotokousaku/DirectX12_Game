#include "DeadParicles.h"

DeadParicles::~DeadParicles() {
	for (DeadParticleLine* deadParticleLine : deadParticleLine_) {
		delete deadParticleLine;
	}
	deadParticleLine_.clear();
	for (DeadParticle* deadParticle : deadBombParticle_) {
		delete deadParticle;
	}
	deadBombParticle_.clear();
}

void DeadParicles::Initialize(Camera* camera, const Vector3& worldPos) {
	camera_ = camera;

	// 発生地点の情報
	emitterData_.worldTransform.Initialize();
	emitterData_.worldTransform.translate = worldPos;
	emitterData_.worldTransform.UpdateMatrix();
	emitterData_.isActive = false;

	// パーティクルの列を生成
	// 列を生成
	int randomNum = (int)GenerateRandomValue(2, 4);
	for (int i = 0; i < randomNum; i++) {
		Vector3 randomVel = GenerateRandomValue(Vector3{ 0.4f,0.4f, 0.4f });
		randomVel.y = GenerateRandomValue(-0.4f, 0.1f);
		DeadParticleLine* deadParticleLine = new DeadParticleLine();
		deadParticleLine->SetDeadParticleTextures(deadParticleTextures_);
		deadParticleLine->Initialize(camera_, randomVel, worldPos);
		deadParticleLine_.push_back(deadParticleLine);
	}

	// 爆発パーティクルの生成
	for (int i = 0; i < kBombParticlesNum; i++) {
		DeadParticle* deadParticle = new DeadParticle();
		deadParticle->SetCamera(camera_);
		deadParticle->SetDeadParticleTextures(deadParticleTextures_);
		deadParticle->SetIsActive(false);
		deadParticle->SetScale(kBombParticlesScale);
		deadParticle->Initialize(emitterData_.worldTransform.GetWorldPosition(), kBombParticlesVelocity);
		deadBombParticle_.push_back(deadParticle);
	}
}

void DeadParicles::Update() {
	if (isFinished_ && deadParticleLine_.empty()) { return; }
	if (!isActive_) { return; }

	emitterData_.worldTransform.UpdateMatrix();

	// パーティクルの列
	for (DeadParticleLine* deadParticleLine : deadParticleLine_) {
		deadParticleLine->Update();
	}

	// 爆発パーティクル
	for (DeadParticle* deadParticle : deadBombParticle_) {
		deadParticle->Update();
	}
	// 1フレームに一つずつ爆発パーティクルを発生
	for (DeadParticle* deadParticle : deadBombParticle_) {
		if (!deadParticle->GetIsActive()) {
			deadParticle->SetWorldPosition(emitterData_.worldTransform.GetWorldPosition());
			deadParticle->SetIsActive(true);
			break;
		}
	}

	// 全てのパーティクルアニメーションが終了しているか
	isFinished_ = ReleaseCheckParticle();
}

void DeadParicles::Draw(ViewProjection viewProjection) {
	for (DeadParticleLine* deadParticleLine : deadParticleLine_) {
		deadParticleLine->Draw(viewProjection);
	}

	for (DeadParticle* deadParticle : deadBombParticle_) {
		deadParticle->Draw(viewProjection);
	}
}

void DeadParicles::CreateParticle(const Vector3& worldPos) {
	isActive_ = true;
	for (DeadParticleLine* deadParticleLine : deadParticleLine_) {
		deadParticleLine->SetEmitterPos(worldPos);
		deadParticleLine->Start();
	}

	emitterData_.worldTransform.translate = worldPos;
}

bool DeadParicles::ReleaseCheckParticle() {
	for (DeadParticleLine* deadParticleLine : deadParticleLine_) {
		if (!deadParticleLine->GetIsFinished()) { 
			return false; 
		}
	}
	return true;
}