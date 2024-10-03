#include "BeamEnemy.h"

BeamEnemy::BeamEnemy() {

}

BeamEnemy::~BeamEnemy() {

}

void BeamEnemy::Initialize(Vector3 pos, Vector3 rotate, int id) {
#pragma region 体のオブジェクト
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
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
	beamObject_->SetModel(models_[0]);
	beamObject_->worldTransform.scale = kDefaultBeamSize;
	beamObject_->worldTransform.UpdateMatrix();
	beamObject_->worldTransform.parent_ = &object3d_->worldTransform;
	// colliderの設定
	beamObject_->collider->SetCollisionPrimitive(kCollisionOBB);
	beamObject_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	beamObject_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	beamObject_->collider->SetOBBLength(beamObject_->worldTransform.scale);
	beamObject_->collider->SetOnCollision(std::bind(&BeamEnemy::OnCollision, this, std::placeholders::_1));
	beamObject_->collider->SetIsActive(true);
#pragma endregion
}

void BeamEnemy::Update() {
	beamObject_->worldTransform.rotate += 0.1f;
}

void BeamEnemy::Draw() {
	// 体
	object3d_->Draw();

	// ビーム
	beamObject_->Draw();
}

Vector3 BeamEnemy::GetWorldPosition() {
	return Vector3();
}

Vector3 BeamEnemy::GetRotation() {
	return Vector3();
}

void BeamEnemy::OnCollision(Collider* collider) {
}
