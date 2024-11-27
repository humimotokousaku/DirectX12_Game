#include "MultiLockOnSystem.h"
#include "GameSystem.h"

void MultiLockOnSystem::Initialize(Player* player, Camera* camera, std::list<IEnemy*>* enemys, GameSystem* gameSystem, Model* model) {
	// シングルトンを種痘
	postEffectManager_ = PostEffectManager::GetInstance();
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();
	// アドレスを取得
	player_ = player;
	camera_ = camera;
	enemys_ = enemys;
	gameSystem_ = gameSystem;
	model_ = model;

	// 射撃SE
	shotSE_ = audio_->SoundLoadWave("Audio/shot.wav");
}

void MultiLockOnSystem::Update() {
	// ロックオンの更新処理
	LockOnUpdate();

	// ロックオンレティクルのスプライト更新
	for (MultiLockOnData& multiLockOnData : multiLockOnDatas_) {
		for (IEnemy* obj : *enemys_) {
			// ロックオン対象外の敵ならスキップ
			if (obj->GetId() != multiLockOnData.enemyId) { continue; }
			// ロックオンレティクルの座標更新
			multiLockOnData.reticleSprite->SetPos(ConvertWorld2Screen(obj->GetWorldPosition()));
		}
		// アニメーションの更新
		multiLockOnData.reticleAnim.Update();
	}

	// 発射処理
	Shot();
}

void MultiLockOnSystem::Draw() {
	for (MultiLockOnData& multiLockOnData : multiLockOnDatas_) {
		multiLockOnData.reticleSprite->Draw();
	}
}

void MultiLockOnSystem::Shot() {
	// RBトリガーを押すと発射
	if (input_->GamePadTrigger(XINPUT_GAMEPAD_RIGHT_SHOULDER) || input_->TriggerKey(DIK_SPACE)) {
		// 弾ゲージが最大でなければ弾を撃てない
		if (!player_->GetBulletGauge().isMax) { return; }
		// 自機の弾が一つもないときに弾を撃てる
		if (gameSystem_->GetPlayerBulletList().size() != 0) {return;}

		// ロックオンリストに登録されている敵に向けて撃つ
		for (MultiLockOnData& multiLockOnData : multiLockOnDatas_) {
			for (IEnemy* obj : *enemys_) {
				// ロックオン対象外の敵ならスキップ
				if (obj->GetId() != multiLockOnData.enemyId) { continue; }

				// 弾を生成し、初期化
				PlayerBullet* newBullet = new PlayerBullet();
				newBullet->SetCamera(camera_);
				newBullet->SetPlayer(player_);
				newBullet->Initialize(model_, player_->GetWorldPosition(), obj->GetWorldTransform());
				// 弾を登録
				gameSystem_->AddPlayerBullet(newBullet);

				// 音の再生
				audio_->SoundPlayWave(shotSE_, false, 0.25f);

				multiLockOnData.isActive = true;
			}
		}
	}

	// 音のこもり具合
	//audio_->SetMuffle(shotSE_, 1.0f);
}

void MultiLockOnSystem::LockOnUpdate() {
	// ロックオンする敵かを検出
	for (IEnemy* obj : *enemys_) {
		// 弾ゲージが最大でなければ敵をロックオンリストに追加しない
		if (!player_->GetBulletGauge().isMax) { break; }
		// ロックオン数が最大数に達していない場合のみ追加
		if (multiLockOnDatas_.size() >= kMaxLockOnNum) { continue; }
		// 自機の正面方向に敵がいないならreturn
		if (IsObjectInOppositeDirection(obj->GetWorldPosition())) { continue; }
		// 画面内にいる
		if (!IsObjectInScreen(obj->GetWorldPosition())) { continue; }
		// ロックオンリストに登録しているなら処理を行わない
		auto result = std::find(lockedEnemyIdList_.begin(), lockedEnemyIdList_.end(), obj->GetId());
		if (result != lockedEnemyIdList_.end()) {
			continue;
		}
		lockedEnemyIdList_.push_back(obj->GetId());

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
		for (IEnemy* enemyItr : *enemys_) {
			if (enemyItr->GetId() != multiLockOnDatas_[i].enemyId) { continue; }

			// 画面内に敵がいない
			if (!IsObjectInScreen(enemyItr->GetWorldPosition()) || IsObjectInOppositeDirection(enemyItr->GetWorldPosition())) {				
				// ロックオン対象のIDを消す
				lockedEnemyIdList_.erase(std::remove(lockedEnemyIdList_.begin(), lockedEnemyIdList_.end(), enemyItr->GetId()), lockedEnemyIdList_.end());
				multiLockOnDatas_.erase(multiLockOnDatas_.begin() + i);
				break;
			}
		}
	}
	// 存在していない敵をロックオンしていたらロックオンリストから削除
	for (int i = 0; i < multiLockOnDatas_.size(); i++) {
		auto result = std::find(enemyIdList_->begin(), enemyIdList_->end(), multiLockOnDatas_[i].enemyId);
		if (result != enemyIdList_->end()) {
			continue;
		}
		lockedEnemyIdList_.erase(std::remove(lockedEnemyIdList_.begin(), lockedEnemyIdList_.end(), multiLockOnDatas_[i].enemyId), lockedEnemyIdList_.end());
		multiLockOnDatas_.erase(multiLockOnDatas_.begin() + i);
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
