#include "AimAssist.h"

AimAssist* AimAssist::GetInstance() {
	static AimAssist instance;
	return &instance;
}

void AimAssist::Initalize() {

}

void AimAssist::Update() {

}

bool AimAssist::IsObjectInOppositeDirection(const Vector3& objectPosition) {
	// カメラの角度方向ベクトルに変換
	Vector3 offset{ 0, 0, 1 };
	// 回転行列を合成
	Matrix4x4 rotateMatrix = MakeRotateMatrix(camera_->worldTransform_.parent_->rotate);
	// 自機のワールド行列の回転を反映する
	offset = TransformNormal(offset, rotateMatrix);

	// 自機と敵の方向ベクトルを算出
	Vector3 p2eDirVel = Normalize(objectPosition - camera_->worldTransform_.translate);

	float dotXZ = Dot(Vector2{ offset.x,offset.z }, Vector2{ p2eDirVel.x,p2eDirVel.z });
	float magnitude1XZ = Length(Vector2{ offset.x,offset.z });
	float magnitude2XZ = Length(Vector2{ p2eDirVel.x,p2eDirVel.z });
	float angleXZ = std::acos(dotXZ / (magnitude1XZ * magnitude2XZ));
	angleXZ = Radian2Degree(angleXZ);

	if ((angleXZ) < (90.0f)) {
		return false;
	}
	// カメラの映らないところにいる
	return true;
}

Vector2 AimAssist::ConvertWorld2Screen(Vector3 worldPos) {
	Vector3 result = worldPos;
	if (result.x == 0 && result.y == 0 && result.z == 0) {
		result = { 0.000001f,0.000001f ,0.000001f };
	}
	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport{};
	matViewProjectionViewport =
		Multiply(camera_->GetViewProjection().matView, Multiply(camera_->GetViewProjection().matProjection, matViewport));
	// ワールド→スクリーン座標変換
	result = Transforms(result, matViewProjectionViewport);

	return Vector2(result.x, result.y);
}

void AimAssist::LockOn() {
	bool isLockOn = false;
	int enemyId = -1;

	for (IEnemy* enemy : enemys_) {
		// ロックオンができる敵か
		if (enemy->GetIsLockOnAvailable()) {
			// 前のフレームでロックオンしている敵がいるorロックオンしていない状態
			// 敵が同じところにいるときにバグるのを回避するため
			if (enemy->GetId() == enemyId_ || enemyId_ <= -1) {
				// 敵がレティクルの一定範囲にいるならロックオン(判定はスクリーン座標で行う)
				Vector2 enemyPos = ConvertWorld2Screen(enemy->GetWorldPosition());
				if (enemyPos.x >= 0.0f && enemyPos.x <= 1280.0f &&
					enemyPos.y >= 0.0f && enemyPos.y <= 720.0f) {
					// カメラの後ろ側ではないかを算出
					if (!IsObjectInOppositeDirection(enemy->GetWorldPosition())) {
						// 自機との距離を調べる
						Vector3 p2e = enemy->GetWorldPosition() - player_->GetWorldPosition();
						// レティクルと自機の距離内にいるならロックオン対象
						if (100.0f >= Length(p2e)) {
							// ロックオンしていないときの3Dレティクルをスクリーン座標に変換
							Vector2 reticle2DPos = ConvertWorld2Screen(player_->GetDefault3DReticlePosition());
							// 2Dレティクルと敵がロックオン範囲内かをスクリーン座標で調べる
							Vector2 r2e = enemyPos - reticle2DPos;
							// ロックオンされてないときの2Dレティクルがある座標とロックオン時の2Dレティクルの座標の距離
							Vector2 r2r = ConvertWorld2Screen(player_->GetWorld3DReticlePosition(0)) - reticle2DPos;
							// 範囲内ならロックオン
							if (kLockOnRange >= Length(Vector3{ r2r.x, r2r.y,0 })) {
								if (kLockOnDisabledDist >= Length(Vector3{ r2e.x, r2e.y,0 })) {
									enemyId = enemy->GetId();
									isLockOn = true;
									// レティクルの補間
									lockOnReticleOffset_ = Lerps::ExponentialInterpolate(lockOnReticleOffset_, enemy->GetWorldPosition() - player_->GetWorld3DReticlePosition(0), 1.0f, 0.5f);
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	// ロックオンが外れた時のレティクルの補間
	if (enemyId <= -1 && !isLockOn) {
		lockOnReticleOffset_ = Lerps::ExponentialInterpolate(lockOnReticleOffset_, player_->GetDefault3DReticlePosition() - player_->GetWorld3DReticlePosition(0), 1.0f, 0.5f);
	}

	// ロックオンフラグに代入
	isLockOn_ = isLockOn;
	// ロックオンした敵の管理番号を代入
	enemyId_ = enemyId;
}
