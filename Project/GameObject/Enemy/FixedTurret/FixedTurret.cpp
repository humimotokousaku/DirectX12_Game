#include "FixedTurret.h"
#include "FixedTurretWaitState.h"
#include "EnemyManager.h"

FixedTurret::FixedTurret() {

}
FixedTurret::~FixedTurret() {
	collisionManager_->ClearColliderList(this);
	models_.clear();
}

void FixedTurret::Initialize(Vector3 pos, Vector3 rotate, int id) {
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

	state_ = new FixedTurretWaitState(this, player_);
	state_->Initialize(this, player_);

	// 管理番号
	id_ = id;
}

void FixedTurret::Update() {
	// 状態遷移
	state_->Update(this);

	// 自機の方向を向く
	// 自機との方向ベクトル
	Vector3 velocity = player_->GetWorldPosition() - GetWorldPosition();
	// Y軸周り角度(θy)
	object3d_->worldTransform.rotate.y = std::atan2(velocity.x, velocity.z);
	// 横軸方向の長さを求める
	float velocityXZ;
	velocityXZ = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	// X軸周りの角度(θx)
	object3d_->worldTransform.rotate.x = std::atan2(-velocity.y, velocityXZ);

	object3d_->worldTransform.UpdateMatrix();
}

void FixedTurret::Draw() {
	// Enemy
	object3d_->Draw(enemyTexture_);
}

void FixedTurret::OnCollision(Collider* collider) {
	if (hp_ <= 0.0f) {
		isDead_ = true;
	}
	else {
		hp_ = hp_ - collider->GetDamage();
	}
}

Vector3 FixedTurret::GetRotation() {
	Vector3 rotate;// = model_->worldTransform.transform.rotate;
	return rotate;
}

Vector3 FixedTurret::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3d_->worldTransform.matWorld_.m[3][0];
	worldPos.y = object3d_->worldTransform.matWorld_.m[3][1];
	worldPos.z = object3d_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}

void FixedTurret::Fire() {
	assert(player_);
	// 自キャラのワールド座標を取得する
	player_->GetWorldPosition();
	// 本体のワールド座標更新
	object3d_->worldTransform.UpdateMatrix();

	// 弾の速度
	const float kBulletSpeed = 1.0f;
	Vector3 velocity{ 0, 0, kBulletSpeed };

	// 速度ベクトルを自機の向きに合わせて回転させる
	velocity = TransformNormal(velocity, object3d_->worldTransform.matWorld_);

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->SetCamera(camera_);
	newBullet->SetPlayer(player_);
	newBullet->SetCollisionManager(collisionManager_);
	newBullet->Initialize(models_[1], object3d_->worldTransform.translate, velocity);

	// 弾を登録
	enemyManager_->AddEnemyBullet(newBullet);
}

void FixedTurret::ChangeState(IFixedTurretState* pState) {
	delete state_;
	state_ = pState;
}