#include "FixedTurret.h"
#include "FixedTurretWaitState.h"
#include "EnemyManager.h"

FixedTurret::FixedTurret() {
	// 体のオブジェクト作成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
}
FixedTurret::~FixedTurret() {
	models_.clear();
}

void FixedTurret::Initialize(Vector3 pos, Vector3 rotate, int id) {
	// 衝突マネージャーのインスタンスを取得
	collisionManager_ = CollisionManager::GetInstance();
	// 敵のテクスチャを割り振る
	enemyTexture_ = TextureManager::GetInstance()->GetSrvIndex("Textures", "Spitfire_Purple.png");

	object3d_->SetCamera(camera_);
	object3d_->SetModel(models_[0]);
	object3d_->worldTransform.translate = pos;
	object3d_->worldTransform.translate = { 0,0,10 };
	object3d_->worldTransform.rotate = rotate;
	object3d_->worldTransform.scale = { 1.5f, 1.5f, 1.5f };
	object3d_->worldTransform.UpdateMatrix();

	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	object3d_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	object3d_->collider->SetOnCollision(std::bind(&FixedTurret::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetIsActive(true);

	// 敵の行動状態
	state_ = new FixedTurretWaitState(this, player_);
	state_->Initialize(this, player_);

	// HP
	hp_ = 50;

	// 管理番号
	id_ = id;

	// スコア
	score_ = 50;

	// 自機と制御点の距離を測ってローカル座標に代入
	for (Vector3& controlPoint : controlPoints_) {
		controlPoint = player_->GetWorldPosition() - controlPoint;
	}
}

void FixedTurret::Update() {
	if (hp_ <= 0.0f) {
		isDead_ = true;
	}

	// 状態遷移
	state_->Update(this);
}

void FixedTurret::Draw() {
	// Enemy
	object3d_->Draw(enemyTexture_);
}

void FixedTurret::OnCollision(Collider* collider) {
	// 自機陣営に当たった場合のみダメージを受ける
	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		hp_ = hp_ - collider->GetDamage();

		// 被弾時のパーティクルを生成
		CreateHitParticle();
	}
}

void FixedTurret::Move() {
	// Catmull-Romスプライン関数で補間された位置を取得
	if (targetT_ <= 1.0f) {
		targetT_ += kMoveSpeed * GameTimer::GetInstance()->GetTimeScale();
		t_ = targetT_ - 0.001f * GameTimer::GetInstance()->GetTimeScale();
	}
	// 注視点を曲線に沿って移動
	target_ = Lerps::CatmullRomSpline(controlPoints_, targetT_);
	Vector3 pos{};
	pos = Lerps::CatmullRomSpline(controlPoints_, t_);
	// 座標を更新
	object3d_->worldTransform.translate = pos;
	object3d_->worldTransform.UpdateMatrix();

	// 自機との方向ベクトル
	Vector3 velocity = object3d_->worldTransform.translate - target_;;
	// Y軸周り角度
	object3d_->worldTransform.rotate.y = std::atan2(velocity.x, velocity.z);
	// 横軸方向の長さを求める
	float velocityXZ;
	velocityXZ = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	// X軸周りの角度
	object3d_->worldTransform.rotate.x = std::atan2(-velocity.y, velocityXZ);
	// Z軸周りの角度
	object3d_->worldTransform.rotate.z = std::atan2(velocity.x, -velocity.z);

	// レールカメラの回転分も加算
	object3d_->worldTransform.rotate += object3d_->worldTransform.parent_->rotate;

	object3d_->worldTransform.UpdateMatrix();
}

void FixedTurret::Aim() {
	// 自機の方向を向く
	// 自機との方向ベクトル
	Vector3 velocity = player_->GetWorldPosition() - GetWorldPosition();
	// Y軸周り角度
	object3d_->worldTransform.rotate.y = std::atan2(velocity.x, velocity.z);
	// 横軸方向の長さを求める
	float velocityXZ;
	velocityXZ = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	// X軸周りの角度
	object3d_->worldTransform.rotate.x = std::atan2(-velocity.y, velocityXZ);

	object3d_->worldTransform.rotate.z = 0.0f;

	// レールカメラの回転分も加算
	object3d_->worldTransform.rotate += object3d_->worldTransform.parent_->rotate;

	object3d_->worldTransform.UpdateMatrix();
}

void FixedTurret::Fire() {
	assert(player_);

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
	newBullet->Initialize(models_[1], GetWorldPosition(), GetWorldTransform());

	// 弾を登録
	enemyManager_->AddEnemyBullet(newBullet);
}

void FixedTurret::ChangeState(IFixedTurretState* pState) {
	delete state_;
	state_ = pState;
}