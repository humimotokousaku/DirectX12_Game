#include "MultiLockOnSystem.h"
#include "GameSystem.h"

void MultiLockOnSystem::Initialize(Player* player, Camera* camera, std::list<BaseEnemy*>* enemys, GameSystem* gameSystem, Model* model) {
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
	// 自機が死んでいるなら処理しない
	if (player_->GetIsDead()) {
		// マルチロックオンの情報
		multiLockOnDatas_.clear();
		// ロックオンされている敵のID
		lockedEnemyIdList_.clear();
		return;
	}

	// ロックオンの更新処理
	LockOnUpdate();

	// ロックオンレティクルのスプライト更新
	for (MultiLockOnData& multiLockOnData : multiLockOnDatas_) {
		for (BaseEnemy* obj : *enemys_) {
			// ロックオン対象外の敵ならスキップ
			if (obj->GetId() != multiLockOnData.enemyId) { continue; }
			// ロックオンレティクルの座標更新
			multiLockOnData.reticleSprite->SetPos(ConvertWorld2Screen(obj->GetWorldPosition()));
		}
		// レティクルアニメーションの更新
		for (Animation& reticleAnim : multiLockOnData.reticleAnim) {
			reticleAnim.Update();
		}
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
		if (gameSystem_->GetPlayerBulletList().size() != 0) { return; }

		if (!isShot_) {
			shotGoal_ = (int)multiLockOnDatas_.size();
			isShot_ = true;
		}
	}

	if (isShot_) {
		if ((int)currentFrame_ % kShotInterval == 0) {
			// ロックオンリストに登録されている敵に向けて撃つ
			for (MultiLockOnData& multiLockOnData : multiLockOnDatas_) {
				// 同じ相手に弾が行かないようにする
				if (multiLockOnData.isActive) { continue; }

				// 発射条件
				for (BaseEnemy* obj : *enemys_) {	
					// ロックオン対象外の敵ならスキップ
					if (obj->GetId() != multiLockOnData.enemyId) { continue; }
					// 弾を生成し、初期化
					PlayerBullet* newBullet = new PlayerBullet();
					newBullet->SetCamera(camera_);
					newBullet->SetPlayer(player_);
					// 発射角度を決める
					shotAngle_ = ShotAngle(shotAngle_);
					newBullet->SetShotAngle(shotAngle_);
					newBullet->Initialize(model_, player_->GetWorldPosition(), obj->GetWorldTransform());
					// 弾を登録
					gameSystem_->AddPlayerBullet(newBullet);

					// 発射回数をカウントする
					shotNum_++;

					// 音の再生
					audio_->SoundPlayWave(shotSE_, false, 0.25f);

					multiLockOnData.isActive = true;
					break;
				}
				// 敵一人に弾を発射するたびにfor文を抜ける
				if (multiLockOnData.isActive) { break; }
			}
		}
		// 経過時間
		currentFrame_ += GameTimer::GetInstance()->GetTimeScale();
	}

	// 全てのロックオン対象に向けて弾を撃つorまた撃てるようにする
	if (shotNum_ >= shotGoal_ || multiLockOnDatas_.empty()) {
		isShot_ = false;
		currentFrame_ = 0;
		shotNum_ = 0;
		shotGoal_ = 0;
		shotAngle_ = { 0,0,0 };
	}

	// 音のこもり具合
	audio_->SetMuffle(shotSE_, 1.0f);
}

void MultiLockOnSystem::LockOnUpdate() {
	// ロックオンする敵かを検出
	for (BaseEnemy* obj : *enemys_) {
		// 弾発射中は処理しない
		if (isShot_) { break; }
		// 弾ゲージが最大でなければ敵をロックオンリストに追加しない
		if (!player_->GetBulletGauge().isMax) { break; }
		// ロックオン数が最大数に達していない場合のみ追加
		if (multiLockOnDatas_.size() >= kMaxLockOnNum) { continue; }
		// 自機の正面方向に敵がいないなら終了
		if (IsObjectInOppositeDirection(obj->GetWorldPosition(), railCamera_->GetWorldTransform().worldPos, railCamera_->GetDirectionVelocity())) { continue; }
		// 画面内にいないなら終了
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
		multiLockOnDatas_[size].reticleSprite->Initialize("Textures", "lockOnReticle.png");
		// 敵の座標にロックオンレティクルを設置
		Vector2 enemyPos = ConvertWorld2Screen(obj->GetWorldPosition());
		multiLockOnDatas_[size].reticleSprite->SetPos(enemyPos);

		// ロックオン時のレティクルのアニメーション
		// レティクルの縮小
		multiLockOnDatas_[size].reticleAnim[0].SetAnimData(multiLockOnDatas_[size].reticleSprite->GetSizeP(), Vector2{ 412.0f,412.0f }, Vector2{ 86.0f,86.0f }, 12, Easings::EaseOutCubic);
		multiLockOnDatas_[size].reticleAnim[0].SetIsStart(true);
		// レティクルの回転
		multiLockOnDatas_[size].reticleAnim[1].SetAnimData(multiLockOnDatas_[size].reticleSprite->GetRotateP(), Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,(float)std::numbers::pi / 2 }, 6, Easings::EaseInCubic);
		multiLockOnDatas_[size].reticleAnim[1].SetIsStart(true);

		// 敵の管理番号
		multiLockOnDatas_[size].enemyId = obj->GetId();
	}

	// ロックオンリストから削除
	EraseLockedList();
}

void MultiLockOnSystem::EraseLockedList() {
#pragma region ロックオンリストから削除
	// 画面内に敵がいない場合
	for (int i = 0; i < multiLockOnDatas_.size(); i++) {
		// 弾発射中は処理しない
		if (isShot_) { break; }
		for (BaseEnemy* enemyItr : *enemys_) {
			if (enemyItr->GetId() != multiLockOnDatas_[i].enemyId) { continue; }

			// 画面内に敵がいない
			if (!IsObjectInScreen(enemyItr->GetWorldPosition()) || IsObjectInOppositeDirection(enemyItr->GetWorldPosition(), railCamera_->GetWorldTransform().worldPos, railCamera_->GetDirectionVelocity())) {
				// ロックオン対象のIDを消す
				lockedEnemyIdList_.erase(std::remove(lockedEnemyIdList_.begin(), lockedEnemyIdList_.end(), enemyItr->GetId()), lockedEnemyIdList_.end());
				multiLockOnDatas_.erase(multiLockOnDatas_.begin() + i);
				break;
			}
		}
	}

	// 存在していない敵をロックオンしていたら削除
	for (int i = 0; i < multiLockOnDatas_.size(); i++) {
		// 弾ゲージが最大でなければロックオンリストをクリア
		if (!player_->GetBulletGauge().isMax) {
			multiLockOnDatas_.clear();
			lockedEnemyIdList_.clear();
			break;
		}

		// 存在していない敵をロックオンしていたらロックオンリストから削除
		auto result = std::find(enemyIdList_->begin(), enemyIdList_->end(), multiLockOnDatas_[i].enemyId);
		if (result != enemyIdList_->end()) {
			continue;
		}
		lockedEnemyIdList_.erase(std::remove(lockedEnemyIdList_.begin(), lockedEnemyIdList_.end(), multiLockOnDatas_[i].enemyId), lockedEnemyIdList_.end());
		multiLockOnDatas_.erase(multiLockOnDatas_.begin() + i);
	}

	// 全ての弾を撃ち切ってもロックオンされている敵がいる場合
	for (int i = 0; i < multiLockOnDatas_.size(); i++) {
		if (multiLockOnDatas_.empty() && !isShot_) { break; }

		if (multiLockOnDatas_[i].isActive) {
			lockedEnemyIdList_.erase(std::remove(lockedEnemyIdList_.begin(), lockedEnemyIdList_.end(), multiLockOnDatas_[i].enemyId), lockedEnemyIdList_.end());
			multiLockOnDatas_.erase(multiLockOnDatas_.begin() + i);
		}
	}
#pragma endregion
}

Vector3 MultiLockOnSystem::ShotAngle(Vector3 shotAngle) {
	shotAngle;
	Vector3 result = {
		-(float)std::numbers::pi / 2.0f,
		0.0f,
		shotAngle.z + 2.0f * (float)std::numbers::pi / (float)shotGoal_,
	};

	return result;
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