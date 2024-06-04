#include "CollisionManager.h"
#include <assert.h>

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	// 衝突フィルタリング
	/*if ((colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) == 0 ||
		(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()) == 0) {
		return;
	}*/

	//// コライダーのワールド座標を取得
	//colliderA->GetWorldPosition();
	//colliderB->GetWorldPosition();

	/// 球体同士の判定
	if (colliderA->GetCollisionPrimitive() == kCollisionSphere && colliderB->GetCollisionPrimitive() == kCollisionSphere) {
		// 座標AとBの距離を求める
		Vector3 a2b = {
			colliderA->GetWorldPosition().x - colliderB->GetWorldPosition().x,
			colliderA->GetWorldPosition().y - colliderB->GetWorldPosition().y,
			colliderA->GetWorldPosition().z - colliderB->GetWorldPosition().z
		};
		float a2bR = colliderA->GetRadius() + colliderB->GetRadius();
		// 球と球の交差判定
		if ((a2b.x * a2b.x) + (a2b.y * a2b.y) + (a2b.z * a2b.z) <= (a2bR * a2bR)) {
			// コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderA);
			// コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderB);

			// 今当たっている
			colliderA->SetIsOnCollision(true);
			colliderB->SetIsOnCollision(true);
		}
		else {
			// 今は当たっていない
			colliderA->SetIsOnCollision(false);
			colliderB->SetIsOnCollision(false);
		}
	}
	/// OBB同士の判定
	if (colliderA->GetCollisionPrimitive() == kCollisionOBB && colliderB->GetCollisionPrimitive() == kCollisionOBB) {
		colliderA->SetOBBCenterPos(colliderA->GetWorldPosition());
		colliderB->SetOBBCenterPos(colliderB->GetWorldPosition());
		for (int i = 0; i < 3; i++) {
			colliderA->SetOBBDirect(i);
			colliderB->SetOBBDirect(i);
		}
		if (ColOBBs(colliderA->GetOBB(), colliderB->GetOBB())) {
			// コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderA);
			// コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderB);

			// 今当たっている
			colliderA->SetIsOnCollision(true);
			colliderB->SetIsOnCollision(true);
		}
		else {
			// 今は当たっていない
			colliderA->SetIsOnCollision(false);
			colliderB->SetIsOnCollision(false);
		}
	}

	// 前のフレームで当たっていたかを更新
	colliderA->SetIsPreOnCollision(colliderA->GetIsOnCollision());
	colliderB->SetIsPreOnCollision(colliderB->GetIsOnCollision());
}

void CollisionManager::CheckAllCollisions() {
	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;

		// イテレータBはイテレータAの次の要素から回す(重複判定を回避)
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;
			// 当たり判定と応答(フレンドリーファイアしないように設定)
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

bool CollisionManager::ColOBBs(const OBB& obb1, const OBB& obb2) {
	// 各方向ベクトルの確保
	// （N***:標準化方向ベクトル）
	Vector3 NAe1 = obb1.m_NormaDirect[0], Ae1 = Multiply(obb1.m_fLength.x, NAe1);
	Vector3 NAe2 = obb1.m_NormaDirect[1], Ae2 = Multiply(obb1.m_fLength.y, NAe2);
	Vector3 NAe3 = obb1.m_NormaDirect[2], Ae3 = Multiply(obb1.m_fLength.z, NAe3);
	Vector3 NBe1 = obb2.m_NormaDirect[0], Be1 = Multiply(obb2.m_fLength.x, NBe1);
	Vector3 NBe2 = obb2.m_NormaDirect[1], Be2 = Multiply(obb2.m_fLength.y, NBe2);
	Vector3 NBe3 = obb2.m_NormaDirect[2], Be3 = Multiply(obb2.m_fLength.z, NBe3);
	Vector3 Interval = Subtract(obb1.m_Pos, obb2.m_Pos);

	// 分離軸 : Ae1
	float rA = Length(Ae1);
	float rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	float L = fabs(Dot(Interval, NAe1));
	if (L > rA + rB) {
		return false; // 衝突していない
	}

	// 分離軸 : Ae2
	rA = Length(Ae2);
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	L = fabs(Dot(Interval, NAe2));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Ae3
	rA = Length(Ae3);
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	L = fabs(Dot(Interval, NAe3));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	rB = Length(Be1);
	L = fabs(Dot(Interval, NBe1));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	rB = Length(Be2);
	L = fabs(Dot(Interval,NBe2));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	rB = Length(Be3);
	L = fabs(Dot(Interval, NBe3));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C11
	Vector3 cross;
	cross = Cross(NAe1, NBe1);
	rA = LenSegOnSeparateAxis(&cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&cross, &Be2, &Be3);
	L = fabs(Dot(Interval, cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C12
	cross = Cross(NAe1, NBe2);
	rA = LenSegOnSeparateAxis(&cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be3);
	L = fabs(Dot(Interval, cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C13
	cross = Cross(NAe1, NBe3);
	rA = LenSegOnSeparateAxis(&cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be2);
	L = fabs(Dot(Interval, cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C21
	cross = Cross(NAe2, NBe1);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&cross, &Be2, &Be3);
	L = fabs(Dot(Interval, cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C22
	cross = Cross(NAe2, NBe2);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be3);
	L = fabs(Dot(Interval, cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C23
	cross = Cross(NAe2, NBe3);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be2);
	L = fabs(Dot(Interval, cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C31
	cross = Cross(NAe3, NBe1);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&cross, &Be2, &Be3);
	L = fabs(Dot(Interval, cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C32
	cross = Cross(NAe3, NBe2);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be3);
	L = fabs(Dot(Interval, cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離軸 : C33
	cross = Cross(NAe3, NBe3);
	rA = LenSegOnSeparateAxis(&cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&cross, &Be1, &Be2);
	L = fabs(Dot(Interval, cross));
	if (L > rA + rB) {
		return false;
	}

	// 分離平面が存在しないので「衝突している」
	return true;
}

float CollisionManager::LenSegOnSeparateAxis(Vector3* Sep, Vector3* e1, Vector3* e2, Vector3* e3){
	// 3つの内積の絶対値の和で投影線分長を計算
	// 分離軸Sepは標準化されていること
	float r1 = fabs(Dot(*Sep, *e1));
	float r2 = fabs(Dot(*Sep, *e2));
	float r3 = e3 ? (fabs(Dot(*Sep, *e3))) : 0;
	return r1 + r2 + r3;
}