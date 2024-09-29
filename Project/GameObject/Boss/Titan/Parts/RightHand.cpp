#include "RightHand.h"

void RightHand::Initialize(const WorldTransform* parent, Camera* camera, Model* model, int id) {
	model_ = model;
	camera_ = camera;

	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetCamera(camera_);
	object3d_->SetModel(model_);
	object3d_->worldTransform.parent_ = parent;
	object3d_->worldTransform.translate = kDefaultPos;
	object3d_->worldTransform.UpdateMatrix();
	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	object3d_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	object3d_->collider->SetOnCollision(std::bind(&RightHand::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetIsActive(true);

	// HP
	hp_ = 50;

	// 管理番号
	id_ = id;

	// スコア
	score_ = 2000;
	
	velocity_ = { 0,0,0 };
}

void RightHand::Update() {
	object3d_->worldTransform.UpdateMatrix();
}

void RightHand::Draw() {
	object3d_->Draw();
}

void RightHand::OnCollision(Collider* collider) {
	if (hp_ <= 0.0f) {
		isDead_ = true;
	}
	else {
		hp_ = hp_ - collider->GetDamage();
	}
}

void RightHand::HandAttack() {
	// 攻撃対象を狙う
	// 自機との方向ベクトル
	Vector3 velocity = player_->GetWorldPosition() - GetWorldPosition();
	if (aimFrame_ <= kAimFrame) {
		// Y軸周り角度(θy)
		//object3d_->worldTransform.rotate.y = -(object3d_->worldTransform.parent_->parent_->parent_->parent_->rotate.y - std::atan2(velocity.x, velocity.z));
		targetRot_.y = std::atan2(velocity.x, velocity.z);
		// 横軸方向の長さを求める
		float velocityXZ;
		velocityXZ = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
		// X軸周りの角度(θx)
		//object3d_->worldTransform.rotate.x = (object3d_->worldTransform.parent_->parent_->parent_->parent_->rotate.x - std::atan2(-velocity.y, velocityXZ));
		targetRot_.x = std::atan2(-velocity.y, velocityXZ);
		object3d_->worldTransform.UpdateMatrix();
	}
	else if (aimFrame_ == kAimFrame + 1) {
		velocity = { 0,0,1 };
		// 速度ベクトルを自機の向きに合わせて回転させる
		Matrix4x4 rotMatrix = MakeRotateMatrix(targetRot_);
		//velocity_ = TransformNormal(velocity, object3d_->worldTransform.matWorld_) * 10;
		velocity_ = TransformNormal(velocity, rotMatrix) * 10;
	}
	else if (aimFrame_ >= kAttackFrame + 15) {
		object3d_->worldTransform.translate = kDefaultPos;
		object3d_->worldTransform.rotate = { 0,0,0 };
		velocity_ = { 0,0,0 };
		targetRot_ = { 0,0,0 };
		aimFrame_ = 0;
	}

	if (aimFrame_ >= kAttackFrame && aimFrame_ <= kAttackFrame + 15) {
		object3d_->worldTransform.translate += velocity_;
	}

	//if (aimFrame_ <= kAttackFrame + 14) {
		aimFrame_++;
	//}

	object3d_->worldTransform.UpdateMatrix();
}

void RightHand::BeamAttack() {

}

void RightHand::WaitState() {

}

void RightHand::RevealWeakPointState() {

}