#include "DeadParticleLine.h"
#include "GameTime.h"

DeadParticleLine::~DeadParticleLine() {
	for (DeadParticle* deadParticle : deadparticleLine_) {
		delete deadParticle;
	}
	deadparticleLine_.clear();
}

void DeadParticleLine::Initialize(Camera* camera, const Vector3& velocity, const Vector3& worldPos) {
	camera_ = camera;

	// 発生地点の情報
	emitterData_.spawnTime = kMaxSpawnTime;
	emitterData_.velocity = velocity;
	emitterData_.worldTransform.Initialize();
	emitterData_.worldTransform.translate = worldPos;
	emitterData_.worldTransform.UpdateMatrix();
	emitterData_.isActive = false;

	for (int i = 0; i < emitterData_.spawnTime; i++) {
		DeadParticle* deadparticle = new DeadParticle();
		deadparticle->SetCamera(camera_);
		deadparticle->SetDeadParticleTextures(deadParticleTextures_);
		deadparticle->Initialize(emitterData_.worldTransform.GetWorldPosition(), Vector3{ 0,0,0 });
		deadparticleLine_.push_back(deadparticle);
	}
}

void DeadParticleLine::Update() {
	if (isFinished_) { return; }

	// パーティクルの更新処理
	ParticleUpdate();
}


void DeadParticleLine::Draw(ViewProjection viewProjection) {
	for (DeadParticle* deadParticle : deadparticleLine_) {
		deadParticle->Draw(viewProjection);
	}
}

void DeadParticleLine::CreateParticle() {
	if (!emitterData_.isActive) { return; }

	for (DeadParticle* deadParticle : deadparticleLine_) {
		if (!deadParticle->GetIsActive()) {
			deadParticle->SetIsActive(true);
			deadParticle->SetWorldPosition(emitterData_.worldTransform.GetWorldPosition());
			break;
		}
	}
}

void DeadParticleLine::ParticleUpdate() {
	// パーティクルの発生条件
	CreateParticle();

	// パーティクルの更新処理
	for (DeadParticle* deadParticle : deadparticleLine_) {
		deadParticle->Update();
	}
	// 全てのパーティクルアニメーションが終了しているか
	isFinished_ = ReleaseCheckParticle();

	// パーティクル発生地点を落下させる
	emitterData_.velocity.y += -0.02f;
	emitterData_.worldTransform.translate += emitterData_.velocity;

	// パーティクルを沸かせる時間が終了したら機能停止
	if (emitterData_.spawnTime <= 0) {
		emitterData_.isActive = false;
	}
	// 発生できる時間を進める
	emitterData_.spawnTime -= GameTimer::GetInstance()->GetTimeScale();

	emitterData_.worldTransform.UpdateMatrix();
}

bool DeadParticleLine::ReleaseCheckParticle() {
	for (DeadParticle* deadParticle : deadparticleLine_) {
		if (!deadParticle->GetIsFinished()) { return false; }
	}
	return true;
}
