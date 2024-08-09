#include "Enemy.h"
#include "EnemyStateWait.h"
#include "EnemyStateApproach.h"
#include "CollisionConfig.h"
#include "EnemyManager.h" 

Enemy::Enemy() {
	
}
Enemy::~Enemy() {
	collisionManager_->ClearColliderList(this);
	models_.clear();
}

void Enemy::Initialize(Vector3 pos, Vector3 rotate,int id) {
	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(~kCollisionAttributeEnemy);
	collisionManager_->SetColliderList(this);

	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetCamera(camera_);
	object3d_->SetModel(models_[0]);
	object3d_->worldTransform.translate = pos;
	object3d_->worldTransform.rotate = rotate;
	object3d_->worldTransform.scale = { 0.5f, 0.5f, 0.5f };

	enemyTexture_ = TextureManager::GetInstance()->GetSrvIndex("Textures", "Spitfire_Purple.png");

	// 状態遷移
	state_ = new EnemyStateApproach();
	state_->Initialize(this, player_);

	// 管理番号
	id_ = id;
}

void Enemy::Update() {
	// 状態遷移
	state_->Update(this);
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
			Particles* particle = new Particles();
			particle->Initialize();
			particle->SetCamera(camera_);
			particle->SetEmitterPos(GetWorldPosition());
			particle->SetEmitterFrequency(1);
			particle->SetEmitterCount(40);
			particle->SetEmitterSpawnCount(1);
			particle->SetRandomPerticle(true);

			enemyManager_->SetHitParticle(particle);
			//hitParticles_.push_back(particle);
		}
	}
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
	newBullet->SetCollisionManager(collisionManager_);
	newBullet->Initialize(models_[1], object3d_->worldTransform.translate, velocity);

	// 弾を登録
	enemyManager_->AddEnemyBullet(newBullet);
}

Vector3 Enemy::GetRotation() {
	Vector3 rotate;// = model_->worldTransform.transform.rotate;
	return rotate;
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};// = model_->worldTransform.transform.translate;
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3d_->worldTransform.matWorld_.m[3][0];
	worldPos.y = object3d_->worldTransform.matWorld_.m[3][1];
	worldPos.z = object3d_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}