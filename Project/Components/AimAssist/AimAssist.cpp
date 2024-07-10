#include "AimAssist.h"

AimAssist* AimAssist::GetInstance() {
	static AimAssist instance;
	return &instance;
}

void AimAssist::Initalize() {

}

void AimAssist::Update() {

}

bool AimAssist::IsEnemyBehindCamera(const Vector3& cameraPosition, const Vector3& cameraDirection, const Vector3& enemyPosition) {
	Vector3 toObject = enemyPosition - cameraPosition;
	Vector3 toObjectNormalized = Normalize(toObject);
	float dot = Dot(cameraDirection, toObjectNormalized);
	// 後ろ側ならtrue
	if (dot < 0.0f) {
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

void AimAssist::LockOn(Vector2 reticlePos) {
	bool isLockOn = false;
	for (Enemy* enemy : enemys_) {
		// カメラの後ろ側ではないかを算出
		if (!IsEnemyBehindCamera(camera_->GetTranslate(), cameraDirVel_, enemy->GetWorldPosition())) {
			// 自機との距離を調べる
			Vector3 p2e = enemy->GetWorldPosition() - player_->GetWorldPosition();
			float dot = Dot(enemy->GetWorldPosition(), player_->GetWorldPosition());
			// レティクルと自機の距離内にいるならロックオン対象
			if (100.0f >= Length(p2e)) {
				// 敵がレティクルの一定範囲にいるならロックオン(判定はスクリーン座標で行う)
				Vector2 enemyPos = ConvertWorld2Screen(enemy->GetWorldPosition());
				// ロックオンしていないときの3Dレティクルをスクリーン座標に変換
				Vector2 reticle2DPos = ConvertWorld2Screen(reticle3DPos_);
				// 2Dレティクルと敵がロックオン範囲内かをスクリーン座標で調べる
				Vector2 r2e = enemyPos - reticle2DPos;
				// ロックオンされてないときの2Dレティクルがある座標とロックオン時の2Dレティクルの座標の距離
				Vector2 r2r = reticlePos - reticle2DPos;
				// 範囲内ならロックオン
				if (96.0f >= Length(Vector3{ r2r.x, r2r.y,0 })) {
					if (96.0f >= Length(Vector3{ r2e.x, r2e.y,0 })) {
						Vector2 e = ConvertWorld2Screen(enemy->GetWorldPosition());
						isLockOn = true;
						player_->SetReticle3DPos(enemy->GetWorldPosition());
					}
				}
			}
		}
	}
	// ロックオンフラグに代入
	isLockOn_ = isLockOn;
}
