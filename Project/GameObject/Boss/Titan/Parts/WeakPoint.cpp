#include "WeakPoint.h"

void WeakPoint::Initialize(const WorldTransform* parent, Camera* camera, Model* model, int id) {
	model_ = model;
	camera_ = camera;

	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetCamera(camera_);
	object3d_->SetModel(model_);
	object3d_->worldTransform.parent_ = parent;
	object3d_->worldTransform.translate = { 0,0,2 };
	object3d_->worldTransform.UpdateMatrix();
	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	object3d_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	object3d_->collider->SetOnCollision(std::bind(&WeakPoint::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetIsActive(true);

	// HP
	hp_ = 100;

	// 管理番号
	id_ = id;

	// スコア
	score_ = 2000;
}

void WeakPoint::Update() {
	object3d_->worldTransform.UpdateMatrix();
}

void WeakPoint::Draw() {
	object3d_->Draw();
}

void WeakPoint::OnCollision(Collider* collider) {
	if (hp_ <= 0.0f) {
		isDead_ = true;
	}
	else {
		hp_ = hp_ - collider->GetDamage();
	}
}
