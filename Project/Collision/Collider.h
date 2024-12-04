#pragma once
#include "CollisionConfig.h"
#include "MathStructs.h"
#include "WorldTransform.h"
#include <functional>
#include <cstdint>

class CollisionManager;
class Collider {
public:
	Collider();
	~Collider();

	// 当たった瞬間を取得
	bool TriggerOnCollision();
	// 離れた瞬間を取得
	bool ReleaseOnCollision();
	// 当たっているときを取得
	bool PressOnCollision();

#pragma region Getter
	// ダメージを取得
	float GetDamage() { return damage_; }

	// 力を取得
	Vector3 GetPower() { return power_; }

	// 半径の取得
	float GetRadius() { return radius_; }

	// OBBの取得
	OBB GetOBB() { return obb_; }

	// 衝突属性(自分)を取得
	const uint32_t& GetCollisionAttribute() { return collisionAttribute_; }
	// 衝突マスク(相手)を取得
	const uint32_t& GetCollisionMask() { return collisionMask_; }
	// 当たり判定の形状を取得
	const uint32_t& GetCollisionPrimitive() { return collisionPrimitive_; }

	// ワールド座標を取得
	Vector3 GetWorldPosition() {
		worldTransform.UpdateMatrix();
		Vector3 worldPos = {
			worldTransform.constMap->matWorld.m[3][0],
			worldTransform.constMap->matWorld.m[3][1],
			worldTransform.constMap->matWorld.m[3][2]
		};
		return worldPos;
	}
	// 角度を取得
	Vector3 GetRotation() { 
		Vector3 rot = worldTransform.rotate;
		if (worldTransform.parent_) {
			rot += worldTransform.parent_->rotate;
		}
		return rot;
	}

	// 今当たっているかを取得
	bool GetIsOnCollision() { return isOnCollision_; }
	// 今当たっているかを取得
	bool GetIsPreOnCollision() { return isPreOnCollision_; }
	// 当たり判定を使用するかを取得
	bool GetIsActive() { return isActive_; }
#pragma endregion

#pragma region Setter
	// ダメージを設定
	void SetDamage(float damage) { damage_ = damage; }

	// 力を設定
	void SetPower(Vector3 power) { power_ = power; }

	// 半径の設定
	void SetRadius(float radius) { radius_ = radius; }

	// OBBの設定
	void SetOBB(OBB obb) { obb_ = obb; }
	void SetOBBCenterPos(Vector3 centerPos) { obb_.m_Pos = centerPos; }
	void SetOBBLength(Vector3 length) { obb_.m_fLength = length; }
	void SetOBBDirect(int index) {
		Matrix4x4 rotate = MakeRotateMatrix(GetRotation());
		obb_.m_NormaDirect[index] = { rotate.m[index][0], rotate.m[index][1], rotate.m[index][2] };
	}

	// 衝突属性(自分)を設定
	void SetCollisionAttribute(const uint32_t& collisionAttribute) { collisionAttribute_ = collisionAttribute; }
	// 衝突マスク(相手)を設定
	void SetCollisionMask(const uint32_t& collisionMask) { collisionMask_ = collisionMask; }
	// 当たり判定の形状を設定
	void SetCollisionPrimitive(const uint32_t& collisionPrimitive) { collisionPrimitive_ = collisionPrimitive; }

	// 今当たっているかを設定
	void SetIsOnCollision(bool isOnCollision) { isOnCollision_ = isOnCollision; }
	// 今当たっているかを設定
	void SetIsPreOnCollision(bool isPreOnCollision) { isPreOnCollision_ = isPreOnCollision; }
	// 当たり判定を使用するかを設定
	void SetIsActive(bool isActive) { isActive_ = isActive; }

	// 衝突応答を設定
	void SetOnCollision(std::function<void(Collider*)> onCollision) { onCollision_ = onCollision; }
#pragma endregion
	
	// 衝突時に呼ばれる関数(ユーザーの使用禁止)
	void OnCollision(Collider* collider) { 
		if (!onCollision_) { return; }
		onCollision_(collider); 
	}

public:// パブリックな変数
	WorldTransform worldTransform;

private:// エンジン機能
	// 衝突マネージャー
	CollisionManager* collisionManager_;

private:// プライベートな変数
	// 衝突応答
	std::function<void(Collider*)> onCollision_;

	// ダメージ
	float damage_;

	// 力
	Vector3 power_;

	// 衝突半径
	float radius_ = 1.0f;
	// OBB
	OBB obb_ = {
		{0,0,0},	// 位置
		{1,1,1},	// 各軸方向の長さ		
		// 方向ベクトル
	   {{1,0,0},
		{0,1,0},
		{0,0,1}}
	};

	// 衝突属性(自分)
	uint32_t collisionAttribute_ = 0xffffffff;
	// 衝突マスク(相手)
	uint32_t collisionMask_ = 0xffffffff;
	// 当たり判定の形状
	uint32_t collisionPrimitive_;

	// コライダーリストから削除
	bool isClear_ = false;
	// 今当たっている
	bool isOnCollision_ = false;
	// 前に当たっている
	bool isPreOnCollision_ = false;
	// 当たり判定を使用
	bool isActive_ = true;
};