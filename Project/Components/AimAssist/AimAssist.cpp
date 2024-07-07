#include "AimAssist.h"

AimAssist* AimAssist::GetInstance() {
	static AimAssist instance;
	return &instance;
}

void AimAssist::Initalize() {

}

void AimAssist::Update() {

}

Vector2 AimAssist::ConvertWorld2Screen(Vector3 worldPos) {
	Vector3 result = worldPos;
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
		// 自機との距離を調べる
		Vector3 p2e = enemy->GetWorldPosition() - player_->GetWorldPosition();
		// レティクルと自機の距離内にいるならロックオン対象
		if (100.0f >= Length(p2e)) {
			// 敵がレティクルの一定範囲にいるならロックオン(判定はスクリーン座標で行う)
			Vector2 enemyPos = ConvertWorld2Screen(enemy->GetWorldPosition());
			// 2Dレティクルと敵がロックオン範囲内かをスクリーン座標で調べる
			Vector2 r2e = enemyPos - reticlePos;
			// 範囲内ならロックオン
			if (64.0f >= Length(Vector3{ r2e.x, r2e.y,0 })) {
				isLockOn = true;
				player_->SetReticle3DPos(enemy->GetWorldPosition());
			}
		}
	}
	// ロックオンフラグに代入
	isLockOn_ = isLockOn;
}
