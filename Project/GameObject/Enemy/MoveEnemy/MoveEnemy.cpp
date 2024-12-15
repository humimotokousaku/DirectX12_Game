#include "MoveEnemy.h"
#include "MoveEnemyStateWait.h"
#include "MoveEnemyStateApproach.h"
#include "CollisionConfig.h"
#include "EnemyManager.h"
#include "GameObjectManager.h"

MoveEnemy::MoveEnemy() {
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
}
MoveEnemy::~MoveEnemy() {
	models_.clear();
}

void MoveEnemy::Initialize(Vector3 pos, Vector3 rotate, int id) {
	// 衝突マネージャーのインスタンスを取得
	collisionManager_ = CollisionManager::GetInstance();

	// 3dオブジェクトの設定
	object3d_->SetCamera(camera_);
	object3d_->SetModel(models_[0]);
	object3d_->worldTransform.translate = pos;
	object3d_->worldTransform.rotate = rotate;
	object3d_->worldTransform.scale = kBodySize;
	object3d_->worldTransform.UpdateMatrix();

	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	object3d_->collider->SetOBBLength(object3d_->worldTransform.scale);
	object3d_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	object3d_->collider->SetOnCollision(std::bind(&MoveEnemy::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetDamage(kBodyDamage);
	object3d_->collider->SetIsActive(true);

	// ゲームオブジェクトマネージャーに追加
	GameObjectManager::GetInstance()->AddGameObject(object3d_.get());

	enemyTexture_ = TextureManager::GetInstance()->GetSrvIndex("Textures", "Spitfire_Purple.png");

	// 状態遷移
	state_ = new MoveEnemyStateApproach();
	state_->Initialize(this, player_);

	// HP
	hp_ = 50;

	// 管理番号
	id_ = id;

	// スコア
	score_ = 100;
}

void MoveEnemy::Update() {
	if (hp_ <= 0.0f) {
		isDead_ = true;
	}

	// Catmull-Romスプライン関数で補間された位置を取得
	Vector3 pos{};
	pos = Lerps::CatmullRomSpline(controlPoints_, t_);
	object3d_->worldTransform.translate = pos;
	// 進行度を進める
	t_ += kMoveSpeed * GameTimer::GetInstance()->GetTimeScale();

	// 状態遷移
	state_->Update(this);

	// ワールドトランスフォームを更新
	object3d_->worldTransform.UpdateMatrix();
}

void MoveEnemy::Draw() {
	// Enemy
	object3d_->Draw(enemyTexture_);
}

void MoveEnemy::OnCollision(Collider* collider) {
	// 自機陣営に当たった場合のみダメージを受ける
	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		// HPを減らす
		hp_ = hp_ - collider->GetDamage();

		// 被弾時のパーティクルを生成
		CreateHitParticle();
	}
}

void MoveEnemy::ChangeState(BaseMoveEnemyState* pState) {
	delete state_;
	state_ = pState;
}

void MoveEnemy::Move(const Vector3 velocity) {
	Vector3 dirVel = TargetOffset(velocity, object3d_->worldTransform.rotate);
	object3d_->worldTransform.translate = object3d_->worldTransform.translate + dirVel;
	object3d_->worldTransform.UpdateMatrix();
}

void MoveEnemy::Fire() {
	assert(player_);

	// 弾の速度(正の数だと敵の後ろから弾が飛ぶ)
	const float kBulletSpeed = -0.5f;
	Vector3 velocity{ 0, 0, kBulletSpeed };

	object3d_->worldTransform.UpdateMatrix();

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->SetCamera(camera_);
	newBullet->SetPlayer(player_);
	newBullet->Initialize(models_[1], GetWorldPosition(), GetWorldTransform());

	// 弾を登録
	enemyManager_->AddEnemyBullet(newBullet);
}