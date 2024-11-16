#include "BeamEnemy.h"
#include "BeamEnemyStateWait.h"
#include "EnemyManager.h" 

BeamEnemy::BeamEnemy() {
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
}

BeamEnemy::~BeamEnemy() {

}

void BeamEnemy::Initialize(Vector3 pos, Vector3 rotate, int id) {
#pragma region 体のオブジェクト
	object3d_->SetCamera(camera_);
	object3d_->SetModel(models_[0]);
	object3d_->worldTransform.translate = pos;
	object3d_->worldTransform.rotate = rotate;
	object3d_->worldTransform.UpdateMatrix();
	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	object3d_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	object3d_->collider->SetOnCollision(std::bind(&BeamEnemy::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetIsActive(true);
#pragma endregion

#pragma region ビームオブジェクト生成
	beamObject_ = std::make_unique<Object3D>();
	beamObject_->Initialize();
	beamObject_->SetCamera(camera_);
	beamObject_->SetModel(models_[1]);
	beamObject_->worldTransform.translate.y = -50;
	beamObject_->worldTransform.scale = kDefaultBeamSize;
	beamObject_->worldTransform.UpdateMatrix();
	beamObject_->worldTransform.parent_ = &object3d_->worldTransform;
	// colliderの設定
	beamObject_->collider->SetCollisionPrimitive(kCollisionOBB);
	beamObject_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	beamObject_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	beamObject_->collider->SetOBBLength(beamObject_->worldTransform.scale);
	beamObject_->collider->SetDamage(30);
	beamObject_->collider->SetOnCollision(std::bind(&BeamEnemy::OnCollision, this, std::placeholders::_1));
	beamObject_->collider->SetIsActive(true);
#pragma endregion

	// 状態遷移
	state_ = new BeamEnemyStateWait(this, player_);
	state_->Initialize();

	// 体力
	hp_ = 100;

	// 管理番号
	id_ = id;
}

void BeamEnemy::Update() {
	// Catmull-Romスプライン関数で補間された位置を取得
	Vector3 pos{};
	pos = Lerps::CatmullRomSpline(controlPoints_, t_);
	object3d_->worldTransform.translate = pos;
	// 進行度を進める
	t_ += 0.005f;

	// ビームを回転させる
	beamObject_->worldTransform.rotate.y += 0.1f;

	object3d_->worldTransform.UpdateMatrix();
}

void BeamEnemy::Draw() {
	// 体
	object3d_->Draw();

	// ビーム
	beamObject_->Draw();
}

void BeamEnemy::ChangeState(IBeamEnemyState* pState) {
	delete state_;
	state_ = pState;
}

void BeamEnemy::OnCollision(Collider* collider) {
	if (hp_ <= 0.0f) {
		isDead_ = true;
	}
	else {
		// 自機陣営に当たった場合のみダメージを受ける
		if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
			// HPを減らす
			hp_ = hp_ - collider->GetDamage();

			// 被弾時のパーティクルを生成
			Particles* particles = new Particles();
			particles->Initialize(GetWorldPosition());
			particles->SetCamera(camera_);
			particles->SetEmitterFrequency(1);
			particles->SetEmitterCount(10);
			particles->SetEmitterSpawnCount(1);
			particles->randomScaleLimit = { 0.01f,0.2f };
			particles->randomVelLimit[0] = { -8.0f,8.0f };
			particles->randomVelLimit[1] = { 0.0f,0.0f };
			particles->randomVelLimit[2] = { -8.0f,8.0f };
			particles->randomColorLimit = { 0.0f,0.0f };
			particles->randomLifeTimeLimit = { 0.1f,0.2f };
			particles->particle_.vel.y = 20.0f;
			particles->particle_.color = { 1.0f,1.0f,1.0f,1.0f };
			particles->SetParticleUpdate(std::bind(&BeamEnemy::HitParticleUpdate, this, std::placeholders::_1));
			enemyManager_->SetHitParticle(particles);
		}
	}
}

void BeamEnemy::HitParticleUpdate(Particle& particle) {
	particle.vel.y -= 2.0f;
}