#include "Enemy.h"
#include "EnemyStateApproach.h"
#include "TextureManager.h"
#include "CollisionConfig.h"
#include "GameScene.h"

Enemy::Enemy() {
	state_ = new EnemyStateApproach();
}
Enemy::~Enemy() {
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
	models_.clear();
}

void Enemy::Initialize() {
	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributeEnemy);

	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetCamera(camera_);
	object3d_->SetModel(models_[0]);

	// 状態遷移
	state_->Initialize(this);
}

void Enemy::Update() {
	// 状態遷移
	state_->Update(this);
}

void Enemy::Draw(uint32_t textureHandle) {
	// Enemy
	object3d_->Draw(textureHandle);
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

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->SetCamera(camera_);
	newBullet->SetPlayer(player_);
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
	Vector3 worldPos;// = model_->worldTransform.transform.translate;
	//// ワールド行列の平行移動成分を取得
	//worldPos.x = model_->worldTransform.matWorld_.m[3][0];
	//worldPos.y = model_->worldTransform.matWorld_.m[3][1];
	//worldPos.z = model_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}