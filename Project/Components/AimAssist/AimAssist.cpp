#include "AimAssist.h"

AimAssist* AimAssist::GetInstance() {
	static AimAssist instance;
	return &instance;
}

void AimAssist::Initalize() {

}

void AimAssist::Update() {

}

bool AimAssist::IsObjectInOppositeDirection(const Vector3& direction, const Vector3& objectPosition)
{
	float dot = Dot(Normalize(direction), Normalize(objectPosition));
	float angle = std::acos(dot); // ラジアンで返す

	// -90° から 90° (ラジアンに変換すると -π/2 ~ π/2) の範囲に含まれていないかを確認
	if (angle > -M_PI / 2 && angle < M_PI / 2) {
		return true;
	}
	return false;
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
		// 前のフレームでロックオンしている敵がいるorロックオンしていない状態
		if (enemy->GetId() == enemyId_ || enemyId_ <= -1) {
			// 敵がレティクルの一定範囲にいるならロックオン(判定はスクリーン座標で行う)
			Vector2 enemyPos = ConvertWorld2Screen(enemy->GetWorldPosition());
			if (enemyPos.x >= 0.0f && enemyPos.x <= 1280.0f &&
				enemyPos.y >= 0.0f && enemyPos.y <= 720.0f) {
				// カメラの後ろ側ではないかを算出
				if (IsObjectInOppositeDirection(*cameraDirVel_, enemy->GetWorldPosition())) {
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
	// ロックオンが外れた時のレティクルの補間
	if (enemyId <= -1 && !isLockOn) {
		lockOnReticleOffset_ = Lerps::ExponentialInterpolate(lockOnReticleOffset_, player_->GetDefault3DReticlePosition() - player_->GetWorld3DReticlePosition(0), 1.0f, 0.5f);
	}

	// ロックオンフラグに代入
	isLockOn_ = isLockOn;
	// ロックオンした敵の管理番号を代入
	enemyId_ = enemyId;
}
