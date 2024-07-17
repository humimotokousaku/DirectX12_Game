#pragma once
#include "Collision/Collider.h"
#include "Collision/CollisionManager.h"
#include "Object3D.h"

class TestCollision : Collider {
public:
	/// 
	/// Default Method
	///

	void Init();
	void Update();
	void Draw();

	///
	/// User Method
	/// 

	void ImGuiParam(const char* name);

#pragma region Setter
	// カメラのアドレスを設定
	void SetCamera(Camera* camera) { camera_ = camera; }
	// 衝突マネージャのアドレスを設定
	void SetCollisionManager(CollisionManager* collisionManager) { collisionManager_ = collisionManager; }

	// 当たり判定の変更
	void SetAABB(AABB aabb) { aabb_ = aabb; }
	// 座標の設定
	void SetPos(Vector3 pos) { 
		object3d_->worldTransform.translate = pos;
		object3d_->worldTransform.UpdateMatrix();
	}
#pragma endregion

	///
	/// 純粋仮想関数
	/// 

	// ワールド座標
	Vector3 GetWorldPosition() override;
	// 角度
	Vector3 GetRotation() override;
	// 衝突判定
	void OnCollision(Collider* collider)override;
private:
	std::unique_ptr<Object3D> object3d_;
	CollisionManager* collisionManager_;
	Camera* camera_;

	AABB aabb_;
};