#include "Titan.h"

Titan::~Titan() {
	models_.clear();
}

void Titan::Initialize(Vector3 pos, Vector3 rotate, int id) {
	// 衝突マネージャーのインスタンスを取得
	collisionManager_ = CollisionManager::GetInstance();

	// 体の生成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetCamera(camera_);
	object3d_->SetModel(models_[0]);
	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	object3d_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	object3d_->collider->SetOnCollision(std::bind(&Titan::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetIsActive(false);

	// パーツの生成
	partsManager_ = std::make_unique<PartsManager>();
	partsManager_->Initialize(&object3d_->worldTransform, camera_, enemyManager_, id);
	partsManager_->SetPlayer(player_);
	partsManager_->SetEnemyManager(enemyManager_);

	// 体の位置と向きを設定
	object3d_->worldTransform.translate = pos;
	object3d_->worldTransform.rotate = rotate;
	object3d_->worldTransform.UpdateMatrix();

	// HP
	hp_ = 1000;

	// 管理番号
	id_ = id;

	// スコア
	score_ = 4000;

	// ロックオンされないようにする
	isLockOnAvailable_ = false;
}

void Titan::Update() {
	partsManager_->Update();
}

void Titan::Draw() {
	// 体
	object3d_->Draw();
	// 他のパーツ
	partsManager_->Draw();
}

void Titan::OnCollision(Collider* collider) {
	if (hp_ <= 0.0f) {
		isDead_ = true;
	}
	else {
		hp_ = hp_ - collider->GetDamage();
	}
}