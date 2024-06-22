#include "PlayerBullet.h"
#include "CollisionConfig.h"
#include "Matrix4x4.h"
#include <cassert>

PlayerBullet::~PlayerBullet() {
	collisionManager_->ClearColliderList(this);
}

void PlayerBullet::Initialize(Model* model, const Vector3& pos, const Vector3& velocity) {
	// モデルの生成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetModel(model);
	object3d_->SetCamera(camera_);

	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
	collisionManager_->SetColliderList(this);

	// ワールド変換の初期化
	object3d_->worldTransform.Initialize();
	// 引数で受け取った初期座標をセット
	object3d_->worldTransform.translate = pos;
	
	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;

	isDead_ = false;
}

void PlayerBullet::Update() {
	// Y軸周り角度(θy)
	object3d_->worldTransform.rotate.y = std::atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ;
	velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	// X軸周りの角度(θx)
	object3d_->worldTransform.rotate.x = std::atan2(-velocity_.y, velocityXZ);

	// 座標を移動させる
	object3d_->worldTransform.translate = Add(object3d_->worldTransform.translate, velocity_);

	//object3d_->worldTransform.translate.z += 0.1f;

	// 行列を更新
	object3d_->worldTransform.UpdateMatrix();

	// 時間経過で死ぬ
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw() {
	// モデルの描画
	object3d_->Draw();
}

void PlayerBullet::OnCollision(Collider* collider) {
	isDead_ = true;
}

Vector3 PlayerBullet::GetRotation()
{
	return Vector3();
}

Vector3 PlayerBullet::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3d_->worldTransform.matWorld_.m[3][0];
	worldPos.y = object3d_->worldTransform.matWorld_.m[3][1];
	worldPos.z = object3d_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}