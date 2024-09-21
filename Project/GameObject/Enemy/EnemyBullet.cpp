#include "EnemyBullet.h"
#include "CollisionConfig.h"
#include "ImGuiManager.h"
#include "WorldTransform.h"
#include "Lerp.h"
#include "Matrix4x4.h"
#include <cassert>

EnemyBullet::~EnemyBullet() {
	
}

void EnemyBullet::Initialize(Model* model, const Vector3& pos, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	// 衝突マネージャーのインスタンスを取得
	collisionManager_ = CollisionManager::GetInstance();

	// モデルを設定
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetCamera(camera_);
	object3d_->SetModel(model);
	object3d_->worldTransform.translate = pos;
	// 形状を設定
	object3d_->worldTransform.scale = {
		0.25f,0.25f,0.8f
	};
	// 色を赤色にする
	object3d_->SetColor(Vector4{ 1.0f,0.0f,0.0f,1.0f });

	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributeEnemy);
	object3d_->collider->SetCollisionMask(~kCollisionAttributeEnemy);
	object3d_->collider->SetOnCollision(std::bind(&EnemyBullet::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetIsActive(true);

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
}

void EnemyBullet::Update() {
	// Y軸周り角度(θy)
	object3d_->worldTransform.rotate.y = std::atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ;
	velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	// X軸周りの角度(θx)
	object3d_->worldTransform.rotate.x = std::atan2(-velocity_.y, velocityXZ);

	// 座標を移動させる
	object3d_->worldTransform.translate = Add(object3d_->worldTransform.translate, velocity_);

	// 行列を更新
	object3d_->worldTransform.UpdateMatrix();

	// 時間経過で死ぬ
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw() {
	// モデルの描画
	object3d_->Draw();
}

void EnemyBullet::OnCollision(Collider* collider) { 
	isDead_ = true;
}

Vector3 EnemyBullet::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3d_->worldTransform.matWorld_.m[3][0];
	worldPos.y = object3d_->worldTransform.matWorld_.m[3][1];
	worldPos.z = object3d_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}

Vector3 EnemyBullet::GetRotation() {
	return object3d_->worldTransform.rotate;
}
