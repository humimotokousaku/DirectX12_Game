#include "Enemy.h"
#include "EnemyStateWait.h"
#include "EnemyStateApproach.h"
#include "CollisionConfig.h"
#include "EnemyManager.h" 

Enemy::Enemy() {

}
Enemy::~Enemy() {
	models_.clear();
}

void Enemy::Initialize(Vector3 pos, Vector3 rotate, int id) {
	// 衝突マネージャーのインスタンスを取得
	collisionManager_ = CollisionManager::GetInstance();

	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetCamera(camera_);
	object3d_->SetModel(models_[0]);
	object3d_->worldTransform.translate = pos;
	object3d_->worldTransform.rotate = rotate;
	object3d_->worldTransform.scale = { 0.5f, 0.5f, 0.5f };
	object3d_->worldTransform.UpdateMatrix();

	// colliderの設定
	bodyCollider_ = std::make_unique<Collider>();
	bodyCollider_->SetCollisionPrimitive(kCollisionOBB);
	bodyCollider_->SetCollisionAttribute(kCollisionAttributeEnemy);
	bodyCollider_->SetCollisionMask(~kCollisionAttributeEnemy);
	bodyCollider_->SetOnCollision(std::bind(&Enemy::OnCollision, this, std::placeholders::_1));
	bodyCollider_->worldTransform.parent_ = &object3d_->worldTransform;
	collisionManager_->SetColliderList(bodyCollider_.get());

	enemyTexture_ = TextureManager::GetInstance()->GetSrvIndex("Textures", "Spitfire_Purple.png");

	// 状態遷移
	state_ = new EnemyStateApproach();
	state_->Initialize(this, player_);

	// HP
	hp_ = 50;

	// 管理番号
	id_ = id;

	// スコア
	score_ = 100;
}

void Enemy::Update() {
	// 状態遷移
	state_->Update(this);

	object3d_->worldTransform.UpdateMatrix();
}

void Enemy::Draw() {
	// Enemy
	object3d_->Draw(enemyTexture_);
}

void Enemy::OnCollision(Collider* collider) {
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
			particles->randomVelLimit[0] = {-8.0f,8.0f};
			particles->randomVelLimit[1] = {0.0f,0.0f};
			particles->randomVelLimit[2] = {-8.0f,8.0f};
			particles->randomColorLimit = { 0.0f,0.0f };
			particles->randomLifeTimeLimit = { 0.1f,0.2f };
			particles->particle_.vel.y = 20.0f;
			particles->particle_.color = { 1.0f,1.0f,1.0f,1.0f };
			particles->SetParticleUpdate(std::bind(&Enemy::ParticleUpdate, this, std::placeholders::_1));
			enemyManager_->SetHitParticle(particles);
		}
	}
}

void Enemy::ParticleUpdate(Particle& particle) {
	particle.vel.y -= 2.0f;
}

void Enemy::ChangeState(IEnemyState* pState) {
	delete state_;
	state_ = pState;
}

void Enemy::Move(const Vector3 velocity) {
	Vector3 dirVel = TargetOffset(velocity, object3d_->worldTransform.rotate);
	object3d_->worldTransform.translate = object3d_->worldTransform.translate + dirVel;
	object3d_->worldTransform.UpdateMatrix();
}

void Enemy::Fire() {
	assert(player_);

	// 弾の速度(正の数だと敵の後ろから弾が飛ぶ)
	const float kBulletSpeed = -0.5f;
	Vector3 velocity{ 0, 0, kBulletSpeed };

	// 自キャラのワールド座標を取得する
	player_->GetWorldPosition();

	object3d_->worldTransform.UpdateMatrix();

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->SetCamera(camera_);
	newBullet->SetPlayer(player_);
	newBullet->Initialize(models_[1], object3d_->worldTransform.translate, velocity);

	// 弾を登録
	enemyManager_->AddEnemyBullet(newBullet);
}

Vector3 Enemy::GetRotation() {
	Vector3 rotate = object3d_->worldTransform.rotate;
	return rotate;
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3d_->worldTransform.matWorld_.m[3][0];
	worldPos.y = object3d_->worldTransform.matWorld_.m[3][1];
	worldPos.z = object3d_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}