#include "MultiLockOnSystem.h"

void MultiLockOnSystem::Initialize(Camera* camera) {
	postEffectManager_ = PostEffectManager::GetInstance();
	aimAssist_ = AimAssist::GetInstance();

	camera_ = camera;
}

void MultiLockOnSystem::Update() {
	// ロックオンの更新処理
	LockOnUpdate();

	for (MultiLockOnData& multiLockOnData : multiLockOnDatas_) {
		// ロックオンレティクルの座標更新
		multiLockOnData.reticleSprite->SetPos(ConvertWorld2Screen(multiLockOnData.worldTransform.GetWorldPosition()));
		// ロックオン対象の座標を更新
		multiLockOnData.worldTransform.UpdateMatrix();
		// アニメーションの更新
		multiLockOnData.reticleAnim.Update();
	}
}

void MultiLockOnSystem::Draw() {
	for (MultiLockOnData& multiLockOnData : multiLockOnDatas_) {
		multiLockOnData.reticleSprite->Draw();
	}
}

void MultiLockOnSystem::LockOnUpdate() {
	// ロックオンする敵かを検出
	for (IEnemy* obj : enemys_) {
		// ロックオン数が最大数に達していない場合のみ追加
		if (multiLockOnDatas_.size() >= kMaxLockOnNum) { continue; }
		// 自機の正面方向に敵がいないならreturn
		if (IsObjectInOppositeDirection(obj->GetWorldPosition())) { continue; }
		// 画面内にいる
		if (!IsObjectInScreen(obj->GetWorldPosition())) { continue; }
		// ロックオンリストに登録しているなら処理を行わない
		auto result = std::find(IDs_.begin(), IDs_.end(), obj->GetId());
		if (result != IDs_.end()) {
			continue;
		}
		IDs_.push_back(obj->GetId());

		// サイズを変更
		int size = (int)multiLockOnDatas_.size();
		multiLockOnDatas_.resize(size + 1);
		// ロックオンした敵の情報を代入
		multiLockOnDatas_[size].worldTransform.Initialize();
		multiLockOnDatas_[size].worldTransform.parent_ = obj->GetWorldTransform();
		multiLockOnDatas_[size].worldTransform.UpdateMatrix();

		// ロックオンスプライトを作成
		multiLockOnDatas_[size].reticleSprite = std::make_shared<Sprite>();
		multiLockOnDatas_[size].reticleSprite->Initialize("", "lockOnReticle.png");
		// 敵の座標にロックオンレティクルを設置
		Vector2 enemyPos = ConvertWorld2Screen(multiLockOnDatas_[size].worldTransform.GetWorldPosition());
		multiLockOnDatas_[size].reticleSprite->SetPos(enemyPos);

		// ロックオン時のレティクルのアニメーション
		multiLockOnDatas_[size].reticleAnim.SetAnimData(multiLockOnDatas_[size].reticleSprite->GetSizeP(), Vector2{ 256,256 }, Vector2{ 86,86 }, 8, Easings::EaseInExpo);
		multiLockOnDatas_[size].reticleAnim.SetIsStart(true);
	}

	// 画面外にいる場合、ロックオンリストから削除
	for (int i = 0; i < multiLockOnDatas_.size(); i++) {
		// 自機の正面方向に敵いるならreturn
		if (!IsObjectInOppositeDirection(multiLockOnDatas_[i].worldTransform.GetWorldPosition())) { continue; }
		// 画面内に敵がいない
		if (!IsObjectInScreen(multiLockOnDatas_[i].worldTransform.GetWorldPosition())) {
			multiLockOnDatas_.erase(multiLockOnDatas_.begin() + i);
		}
	}
}

Vector2 MultiLockOnSystem::ConvertWorld2Screen(Vector3 worldPos) {
	Vector3 result = worldPos;
	result += { 0.000001f,0.000001f ,0.000001f };

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

bool MultiLockOnSystem::IsObjectInScreen(Vector3 worldPos) {
	// 敵がレティクルの一定範囲にいるならロックオン(判定はスクリーン座標で行う)
	Vector2 objectPos = ConvertWorld2Screen(worldPos);
	// 対象が画面に映っているか
	if (objectPos.x >= 0.0f && objectPos.x <= 1280.0f &&
		objectPos.y >= 0.0f && objectPos.y <= 720.0f) {
		return true;
	}
	return false;
}

bool MultiLockOnSystem::IsObjectInOppositeDirection(const Vector3& objectPosition) {
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
