#include "BaseEnemy.h"
#include "EnemyManager.h"

void BaseEnemy::CreateHitParticle() {
	// パーティクルを生成
	Particles* particles = new Particles();
	particles->Initialize(GetWorldPosition());
	particles->SetCamera(camera_);
	// 粒子の発生頻度
	particles->SetEmitterFrequency(1);
	// 一度に発生する粒子の数
	particles->SetEmitterCount(10);
	// 粒子を出す回数
	particles->SetEmitterSpawnCount(1);
	particles->randomScaleLimit = { 0.01f,0.2f };
	particles->randomVelLimit[0] = { -8.0f,8.0f };
	particles->randomVelLimit[1] = { 0.0f,0.0f };
	particles->randomVelLimit[2] = { -8.0f,8.0f };
	particles->randomColorLimit = { 0.0f,0.0f };
	particles->randomLifeTimeLimit = { 0.1f,0.2f };
	particles->particle_.vel.y = 20.0f;
	particles->particle_.color = { 1.0f,1.0f,1.0f,1.0f };
	particles->SetParticleUpdate(std::bind(&BaseEnemy::HitParticleUpdate, this, std::placeholders::_1));
	enemyManager_->SetHitParticle(particles);
}

void BaseEnemy::HitParticleUpdate(Particle& particle) {
	particle.vel.y -= 2.0f;
}