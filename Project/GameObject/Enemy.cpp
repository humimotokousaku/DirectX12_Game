#include "Enemy.h"
#include "TextureManager.h"
#include "Collision/CollisionConfig.h"

Enemy::~Enemy() {
	//delete model_;
}

void Enemy::Initialize(Camera* camera) {
	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);

	//model_ = std::make_unique<Object3D>();
	//model_->Initialize();
	//model_->SetModel("", "block.obj");
	//model_->SetCamera(camera);
}

void Enemy::Update() {

}

void Enemy::Draw(uint32_t textureHandle) {
	// Enemy
	//model_->Draw(textureHandle);
}

void Enemy::OnCollision(Collider* collider) {

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