#include "Collider.h"
#include "CollisionManager.h"

Collider::Collider() {
	// 衝突マネージャーのインスタンスを取得
	collisionManager_ = CollisionManager::GetInstance();
	// 当たり判定のワールドトランスフォームの初期化
	worldTransform.Initialize();
}

Collider::~Collider() {
	collisionManager_->ClearColliderList(this);
}

bool Collider::TriggerOnCollision() {
	if (!isPreOnCollision_ && isOnCollision_) {
		return true;
	}
	else {
		return false;
	}
}

bool Collider::ReleaseOnCollision() {
	if (isPreOnCollision_ && !isOnCollision_) {
		return true;
	}
	else {
		return false;
	}
}

bool Collider::PressOnCollision() {
	if (isOnCollision_) {
		return true;
	}
	else {
		return false;
	}
}