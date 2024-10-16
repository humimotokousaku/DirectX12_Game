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

	for (IEnemy* enemy : enemys_) {
		// ロックオンができる敵か
		if (!enemy->GetIsLockOnAvailable()) { continue; }
		// カメラの後ろ側ではないかを算出
		if (IsObjectInOppositeDirection(enemy->GetWorldPosition())) { continue; }

		// 敵がレティクルの一定範囲にいるならロックオン(判定はスクリーン座標で行う)
		Vector2 enemyPos = ConvertWorld2Screen(enemy->GetWorldPosition());
		// 敵が画面に映っているか
		if (enemyPos.x >= 0.0f && enemyPos.x <= 1280.0f &&
			enemyPos.y >= 0.0f && enemyPos.y <= 720.0f) {
			// 自機と敵の距離
			Vector3 p2e = enemy->GetWorldPosition() - player_->GetWorldPosition();
			// レティクルと自機の距離内にいるならロックオン対象
			if (kLockOnDistance >= Length(p2e)) {
				// ロックオンしていないときの3Dレティクルをスクリーン座標に変換
				Vector2 default2DReticlePos = ConvertWorld2Screen(player_->GetDefault3DReticlePosition());

				// ロックオンしていない時の2Dレティクルと敵の距離をスクリーン座標で算出
				Vector2 r2e = enemyPos - default2DReticlePos;
				// ロックオンされてない時の2Dレティクルがある座標とロックオン時の2Dレティクルの座標の距離
				Vector2 r2r = ConvertWorld2Screen(player_->GetWorld3DReticlePosition(0)) - default2DReticlePos;

				// ロックオン解除範囲をレティクルが超えているならロックオン終了
				if (kLockOnDisabledDist < Length(Vector3{ r2e.x, r2e.y,0 })) { continue; }
				// 範囲内ならロックオン
				if (kLockOnRange >= Length(Vector3{ r2r.x, r2r.y,0 })) {
					isLockOn = true;
					// レティクルの補間
					lockOnReticleOffset_ = Lerps::ExponentialInterpolate(lockOnReticleOffset_, enemy->GetWorldPosition() - player_->GetWorld3DReticlePosition(0), 1.0f, 0.5f);
					break;
				}
			}
		}
	}
	// ロックオンが外れた時のレティクルの補間
	if (!isLockOn) {
		lockOnReticleOffset_ = Lerps::ExponentialInterpolate(lockOnReticleOffset_, player_->GetDefault3DReticlePosition() - player_->GetWorld3DReticlePosition(0), 1.0f, 0.5f);
	}

	// ロックオンフラグに代入
	isLockOn_ = isLockOn;
}
