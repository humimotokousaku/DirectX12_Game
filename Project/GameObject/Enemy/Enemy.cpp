#include "Enemy.h"
#include "EnemyStateApproach.h"
#include "TextureManager.h"
#include "CollisionConfig.h"
#include "GameScene.h"

Enemy::Enemy() {
	state_ = new EnemyStateApproach();
}
Enemy::~Enemy() {
	collisionManager_->ClearColliderList(this);
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
	models_.clear();
}

void Enemy::Initialize(Vector3 pos) {
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

	// 状態遷移
	state_->Initialize(this);
}

void Enemy::Update() {
	// 状態遷移
	state_->Update(this);
}

void Enemy::Draw() {
	// Enemy
	object3d_->Draw();
}

void Enemy::OnCollision(Collider* collider) {

}

void Enemy::ChangeState(IEnemyState* pState) {
	delete state_;
	state_ = pState;
}

void Enemy::Move(const Vector3 velocity) {
	object3d_->worldTransform.translate = Add(object3d_->worldTransform.translate, velocity);
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
	gameScene_->AddEnemyBullet(newBullet);
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