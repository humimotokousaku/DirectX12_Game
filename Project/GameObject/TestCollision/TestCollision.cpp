#include "TestCollision.h"

void TestCollision::Init() {
	// colliderの設定
	SetCollisionPrimitive(kCollisionAABB);
	SetAABB(aabb_);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
	collisionManager_->SetColliderList(this);

	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetModel("", "block.obj");
	object3d_->SetCamera(camera_);
	//object3d_->worldTransform.translate = { -6,0,10 };
	object3d_->worldTransform.UpdateMatrix();
}

void TestCollision::Update() {

}

void TestCollision::Draw() {
	object3d_->Draw();
}

void TestCollision::ImGuiParam(const char* name) {
	object3d_->ImGuiParameter(name);
}

void TestCollision::OnCollision(Collider* collider) {
	// 衝突した相手の情報取得
	AABB aabb = collider->GetAABB();

	// 衝突したオブジェクトとの方向ベクトル
	Vector3 dirVel = GetWorldPosition() - collider->GetWorldPosition();

	// minを正数に直す
	aabb.min.x = fabsf(aabb.min.x);
	aabb.min.y = fabsf(aabb.min.y);
	aabb.min.z = fabsf(aabb.min.z);

	Vector3 c = aabb.min + GetAABB().max;
	if (dirVel.x >= c.x) {
		return;
	}
	if (dirVel.y >= c.y) {
		return;
	}
	if (dirVel.z >= c.z) {
		return;
	}

	if (collider->GetWorldPosition().x < GetWorldPosition().x) {
		c.x = aabb.max.x + -GetAABB().min.x;
	}
	if (collider->GetWorldPosition().y < GetWorldPosition().y) {
		c.y = aabb.max.y + -GetAABB().min.y;
	}
	if (collider->GetWorldPosition().z < GetWorldPosition().z) {
		c.z = aabb.max.z + -GetAABB().min.z;
	}

	Vector3 a = c - dirVel;
	dirVel = dirVel - a;

	// 押し出し処理
	object3d_->worldTransform.translate = object3d_->worldTransform.translate - dirVel;
	object3d_->worldTransform.UpdateMatrix();
}

Vector3 TestCollision::GetRotation() {
	Vector3 rotate = object3d_->worldTransform.rotate;
	return rotate;
}

Vector3 TestCollision::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos = object3d_->worldTransform.translate;
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3d_->worldTransform.matWorld_.m[3][0];
	worldPos.y = object3d_->worldTransform.matWorld_.m[3][1];
	worldPos.z = object3d_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}