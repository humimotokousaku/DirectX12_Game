#include "MultiLockOnSystem.h"
#include "GameSystem.h"

void MultiLockOnSystem::Initialize(Player* player, Camera* camera, std::list<IEnemy*> enemys, GameSystem* gameSystem, Model* model) {
	postEffectManager_ = PostEffectManager::GetInstance();

	player_ = player;
	camera_ = camera;
	enemys_ = enemys;
	gameSystem_ = gameSystem;
	model_ = model;
}

void MultiLockOnSystem::Update() {
	// ロックオンの更新処理
	LockOnUpdate();

	for (MultiLockOnData& multiLockOnData : multiLockOnDatas_) {
		for (IEnemy* obj : enemys_) {
			// ロックオン対象外の敵ならスキップ
			if (obj->GetId() != multiLockOnData.enemyId) { continue; }

			// ロックオンレティクルの座標更新
			multiLockOnData.reticleSprite->SetPos(ConvertWorld2Screen(obj->GetWorldPosition()));
		}

		// アニメーションの更新
		multiLockOnData.reticleAnim.Update();
	}



	// 弾ゲージが0以外で発射クールタイムが0のとき
//if (bulletInterval_ <= 0 && bulletGauge_.value >= kDecrementBulletGauge) {
	for (MultiLockOnData& multiLockOnData : multiLockOnDatas_) {
		if (multiLockOnData.isActive) { continue; }
		multiLockOnData.isActive = true;
		for (IEnemy* obj : enemys_) {
			// ロックオン対象外の敵ならスキップ
			if (obj->GetId() != multiLockOnData.enemyId) { continue; }

			// 弾の速度
			const float kBulletSpeed = 5.0f;
			Vector3 velocity(0, 0, kBulletSpeed);
			// 自機の座標
			Vector3 worldPos = player_->GetWorldPosition();

			Matrix4x4 m = player_->GetWorldTransform()->matWorld_;
			// 速度ベクトルを自機の向きに合わせて回転させる
			velocity = TransformNormal(velocity, m);
			// 自機から照準オブジェクトへのベクトル
			Vector3 targetPos = obj->GetWorldPosition();
			velocity = targetPos - worldPos;
			velocity = Normalize(velocity);
			velocity *= kBulletSpeed;

			// 弾を生成し、初期化
			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->SetCamera(camera_);
			newBullet->Initialize(model_, worldPos, velocity, obj->GetWorldTransform());
			// 弾を登録
			gameSystem_->AddPlayerBullet(newBullet);

			// 弾の発射間隔リセット
			//bulletInterval_ = kBulletInterval;

			// 弾ゲージを減少させる
			//bulletGauge_.value -= kDecrementBulletGauge;

			// 音の再生
			//audio_->SoundPlayWave(shotSE_, false, 0.25f);

			if (multiLockOnData.isActive) { break; }
		}
		if (multiLockOnData.isActive) { break; }
	}
	//}

	//bulletInterval_--;
	//// 0未満にならないようにする
	//bulletInterval_ = std::clamp<int>(bulletInterval_, 0, kBulletInterval);
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

		// ロックオンリストに登録
		// サイズを変更
		int size = (int)multiLockOnDatas_.size();
		multiLockOnDatas_.resize(size + 1);

		// ロックオンスプライトを作成
		multiLockOnDatas_[size].reticleSprite = std::make_shared<Sprite>();
		multiLockOnDatas_[size].reticleSprite->Initialize("", "lockOnReticle.png");
		// 敵の座標にロックオンレティクルを設置
		Vector2 enemyPos = ConvertWorld2Screen(obj->GetWorldPosition());
		multiLockOnDatas_[size].reticleSprite->SetPos(enemyPos);

		// ロックオン時のレティクルのアニメーション
		multiLockOnDatas_[size].reticleAnim.SetAnimData(multiLockOnDatas_[size].reticleSprite->GetSizeP(), Vector2{ 256,256 }, Vector2{ 86,86 }, 6, Easings::EaseInExpo);
		multiLockOnDatas_[size].reticleAnim.SetIsStart(true);

		// 敵の管理番号
		multiLockOnDatas_[size].enemyId = obj->GetId();
	}

	// ロックオンリストから削除
	for (int i = 0; i < multiLockOnDatas_.size(); i++) {
		for (IEnemy* enemyItr : enemys_) {
			if (enemyItr->GetId() != multiLockOnDatas_[i].enemyId) { continue; }

			// 画面内に敵がいない
			if (!IsObjectInScreen(enemyItr->GetWorldPosition()) || IsObjectInOppositeDirection(enemyItr->GetWorldPosition())) {
				multiLockOnDatas_.erase(multiLockOnDatas_.begin() + i);
				// ロックオン対象のIDを消す
				IDs_.erase(std::remove(IDs_.begin(), IDs_.end(), enemyItr->GetId()), IDs_.end());
				break;
			}
		}
	}

	// 存在していない敵をロックオンしていたらロックオンリストから削除
	for (int i = 0; i < multiLockOnDatas_.size(); i++) {
		auto result = std::find(enemyIdList_.begin(), enemyIdList_.end(), multiLockOnDatas_[i].enemyId);
		if (result != enemyIdList_.end()) {
			continue;
		}
		multiLockOnDatas_.erase(multiLockOnDatas_.begin() + i);
		IDs_.erase(std::remove(IDs_.begin(), IDs_.end(), multiLockOnDatas_[i].enemyId), IDs_.end());
	}
}

Vector2 MultiLockOnSystem::ConvertWorld2Screen(Vector3 worldPos) {
	Vector3 result = worldPos;
	result += { 0.000001f, 0.000001f, 0.000001f };

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
