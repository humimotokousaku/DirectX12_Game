#include "EnemyBullet.h"
#include "CollisionConfig.h"
#include "ImGuiManager.h"
#include "WorldTransform.h"
#include "Lerp.h"
#include "Matrix4x4.h"
#include <cassert>

EnemyBullet::~EnemyBullet() {
	collisionManager_->ClearColliderList(this);
}

void EnemyBullet::Initialize(Model* model, const Vector3& pos, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	// モデルを設定
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetCamera(camera_);
	object3d_->SetModel(model);
	object3d_->worldTransform.translate = pos;

	// 形状を設定
	SettingScale();

	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(~kCollisionAttributeEnemy);
	collisionManager_->SetColliderList(this);

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
}

void EnemyBullet::Update() {
//	Vector3 toPlayer = Subtract(player_->GetWorldPosition(), object3d_->worldTransform.translate);
//	toPlayer = Normalize(toPlayer);
//	velocity_ = Normalize(velocity_);
//	// 球面線形保管により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
//	velocity_ = Lerps::Slerp(velocity_, toPlayer, 0.1f);
//	velocity_.x *= 0.5f;
//	velocity_.y *= 0.5f;
//	velocity_.z *= 0.5f;
//
//#pragma region 弾の角度
//
//	// Y軸周り角度(θy)
//	object3d_->worldTransform.rotate.y = std::atan2(velocity_.x, velocity_.z);
//	// 横軸方向の長さを求める
//	float velocityXZ;
//	velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
//	// X軸周りの角度(θx)
//	object3d_->worldTransform.rotate.x = std::atan2(-velocity_.y, velocityXZ);
//
//#pragma endregion
//
//	// 座標を移動させる
//	object3d_->worldTransform.translate = Add(object3d_->worldTransform.translate, velocity_);

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

void EnemyBullet::SettingScale() {
	object3d_->worldTransform.scale.x = 1.0f;
	object3d_->worldTransform.scale.y = 1.0f;
	object3d_->worldTransform.scale.z = 1.0f;
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

Vector3 EnemyBullet::GetRotation()
{
	return Vector3();
}
