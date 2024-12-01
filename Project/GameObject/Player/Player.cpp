#include "Player.h"
#include "CollisionConfig.h"
#include "GameSystem.h"
#include "GameTime.h"
#include "GlobalVariables.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "PostEffectManager.h"
#include "TextureManager.h"
#include "Utility.h"
#include "PointLight.h"
#include <numbers>

Player::Player() {}
Player::~Player() {
	models_.clear();
}

void Player::Initialize() {
	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	collisionManager_ = CollisionManager::GetInstance();
	gameTimer_ = GameTimer::GetInstance();

#pragma region 読み込み
	TextureManager::GetInstance()->LoadTexture("Level", "gray.png");

	// 自機のテクスチャ
	playerTexture_ = TextureManager::GetInstance()->GetSrvIndex("Textures", "Bob_Red.png");
	deadParticleTexture = TextureManager::GetInstance()->GetSrvIndex("Level", "gray.png");
	defaultTexture = TextureManager::GetInstance()->GetSrvIndex("Textures/DefaultTexture", "white.png");
#pragma endregion

#pragma region 自機モデル作成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetModel(models_[0]);
	object3d_->SetCamera(camera_);
	object3d_->worldTransform.scale = { 0.5f,0.5f,0.5f };
	object3d_->worldTransform.UpdateMatrix();
	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributePlayer);
	object3d_->collider->SetCollisionMask(~kCollisionAttributePlayer);
	object3d_->collider->SetOnCollision(std::bind(&Player::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetIsActive(true);
	// ポイントライトを追従させる
	PointLight::GetInstance()->SetTargetPosition(&object3d_->worldTransform.worldPos);
	PointLight::GetInstance()->SetTargetOffset(Vector3{ 0,2,0 });
	PointLight::GetInstance()->SetRadius(1500.0f);

	// 自機の残像オブジェクトを作成
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		afterImageObject3d_[i] = std::make_unique<Object3D>();
		afterImageObject3d_[i]->Initialize();
		afterImageObject3d_[i]->SetModel(models_[0]);
		afterImageObject3d_[i]->SetCamera(camera_);
		afterImageObject3d_[i]->worldTransform.scale = { 0.5f,0.5f,0.5f };
		afterImageObject3d_[i]->worldTransform.UpdateMatrix();
		// 透明にする
		afterImageObject3d_[i]->SetColor(Vector4{ 0.2f,0.2f,0.2f,0.3f });
		// 表示をしない
		afterImageObject3d_[i]->SetIsActive(false);
	}
#pragma endregion

#pragma region ゲージ系のUI作成
	// HP量
	hp_.value = kMaxHp;
	// HPバーのサイズ指定
	hp_.size = kMaxHPSize;
	hp_.sprite = std::make_unique<Sprite>();
	hp_.sprite->Initialize("Textures/DefaultTexture", "white.png");
	hp_.sprite->SetSize(hp_.size);
	hp_.sprite->SetAnchorPoint(Vector2{ 0.0f,0.5f });
	hp_.sprite->SetPos(Vector2{ 64.0f, 64.0f });
	// 緑色にする
	hp_.sprite->SetColor(Vector4{ 0.0f,1.0f,0.0f,1.0f });
	PostEffectManager::GetInstance()->AddSpriteList(hp_.sprite.get());

	// 弾ゲージ量
	bulletGauge_.value = 0.0f;
	//bulletGauge_.value = kMaxBulletGauge;
	bulletGauge_.incrementValue = kIncrementBulletGauge;
	// 弾ゲージの増加倍率
	bulletGauge_.magnification = kMagnificationBulletGauge;
	// 弾ゲージのサイズ指定
	bulletGauge_.size = kMaxBulletGaugeSize;
	bulletGauge_.sprite = std::make_unique<Sprite>();
	bulletGauge_.sprite->Initialize("Textures/DefaultTexture", "white.png");
	bulletGauge_.sprite->SetSize(bulletGauge_.size);
	bulletGauge_.sprite->SetAnchorPoint(Vector2{ 0.0f,0.5f });
	bulletGauge_.sprite->SetPos(Vector2{ 64.0f, 96.0f });
	// 青色にする
	bulletGauge_.sprite->SetColor(Vector4{ 0.0f,0.0f,1.0f,1.0f });
	PostEffectManager::GetInstance()->AddSpriteList(bulletGauge_.sprite.get());
#pragma endregion

#pragma region パーティクル
	// 自機の軌道パーティクルの作成
	for (int i = 0; i < particles_.size(); i++) {
		particles_[i] = std::make_unique<Particles>();
		particles_[i]->Initialize(GetWorldPosition());
		particles_[i]->SetCamera(camera_);
		particles_[i]->SetEmitterParent(&object3d_->worldTransform);
		// 発生頻度
		particles_[i]->SetEmitterFrequency(1.0f / 240.0f);
		// 一度に発生する個数
		particles_[i]->SetEmitterCount(1);
		// ランダムを切る
		particles_[i]->OffRandom();
		// パーティクル一粒の詳細設定
		particles_[i]->particle_.color = { 1,1,1,0.6f };
		particles_[i]->particle_.lifeTime = 30.0f;
		particles_[i]->particle_.transform.translate = { 0.0f,0.0f,0.0f };
		particles_[i]->particle_.transform.scale = { 0.1f,0.1f,0.1f };
		particles_[i]->particle_.vel = { 0.0f,0.0f,1.0f };
	}

	// 死亡時のパーティクル
	deadParticle_ = std::make_unique<Particles>();
	deadParticle_->Initialize(GetWorldPosition());
	deadParticle_->SetCamera(camera_);
	deadParticle_->SetEmitterParent(&object3d_->worldTransform);
	// 発生頻度
	deadParticle_->SetEmitterFrequency(1.0f / 240.0f);
	// 一度に発生する個数
	deadParticle_->SetEmitterCount(40);
	// ランダムを切る
	// パーティクル一粒の詳細設定
	deadParticle_->SetEmitterSpawnCount(0);
	deadParticle_->particle_.vel = { 0.0f,0.0f,10.0f };
	deadParticle_->randomScaleLimit = { 0.1f,0.3f };
	deadParticle_->randomVelLimit[0] = { -2.0f,2.0f };
	deadParticle_->randomVelLimit[1] = { 0.0f,5.0f };
	deadParticle_->randomColorLimit = { 0.0f,0.0f };
	deadParticle_->randomLifeTimeLimit = { 1.1f,2.2f };
	float color = 1.0f;
	deadParticle_->particle_.color = { color,color,color,1.0f };

	// ジャスト回避時のパーティクル
	justEvasionParticle_ = std::make_unique<Particles>();
	justEvasionParticle_->Initialize(GetWorldPosition());
	justEvasionParticle_->SetCamera(camera_);
	justEvasionParticle_->SetEmitterParent(&object3d_->worldTransform);
	justEvasionParticle_->SetParticleUpdate(std::bind(&Player::JustParticleUpdate, this, std::placeholders::_1));
	// 発生頻度
	justEvasionParticle_->SetEmitterFrequency(1.0f / 240.0f);
	// 一度に発生する個数
	justEvasionParticle_->SetEmitterCount(40);
	// パーティクル一粒の詳細設定
	justEvasionParticle_->SetEmitterSpawnCount(1);
	justEvasionParticle_->randomScaleLimit = { 0.1f,0.3f };
	justEvasionParticle_->randomTranslateLimit = { -1.0f,1.0f };
	justEvasionParticle_->randomVelLimit[0] = { -2.0f,2.0f };
	justEvasionParticle_->randomVelLimit[1] = { -2.0f,2.0f };
	justEvasionParticle_->randomVelLimit[2] = { -2.0f,2.0f };
	justEvasionParticle_->randomColorLimit = { 0.0f,0.0f };
	justEvasionParticle_->randomLifeTimeLimit = { 10.1f,20.2f };
	justEvasionParticle_->particle_.color = { 0.0f,0.0f,1.0f,1.0f };
#pragma endregion

#pragma region アニメーションの設定
	// ブースト時の自機回転アニメーション
	boostRotAnim_.SetAnimData(&boost_.rotVelZ, 0.0f, float{ 4.0f * M_PI }, 90, Easings::EaseOutExpo);

	// 回避時の移動速度のイージング
	evasionSpeedAnim_.SetAnimData(&evasion_.moveSpeed, kMaxEvasionMoveSpeed, Vector2{ 0.0f,0.0f }, kMaxEvasionFrame, Easings::EaseOutExpo);
	// 回避時の回転速度のイージング
	evasionRotSpeedAnim_.SetAnimData(&evasion_.rotVel, Vector3{ 0.0f,0.0f, 0.0f }, Vector3{ 0.0f, 0.0f ,(2.0f * (float)std::numbers::pi) * 4.0f }, 40, Easings::EaseOutExpo);
	// 残像のα値のアニメーション
	evasionAlphaAnims_.resize(afterImageObject3d_.size());
	evasion_.alphas.resize(evasionAlphaAnims_.size());
	for (int i = 0; i < evasionAlphaAnims_.size(); i++) {
		evasionAlphaAnims_[i].SetAnimData(&evasion_.alphas[i], 1.0f, 0.0f, 20, Easings::EaseOutExpo);
	}

	// 死亡アニメーション
	// 自機を下に向ける
	deadAnim_.animation.SetAnimData(&deadAnim_.rotate.x, 0.0f, (float)std::numbers::pi / 2.0f, 120, Easings::EaseInQuart);
#pragma endregion

	// ジャスト回避時の文字
	justSprite_ = std::make_unique<Sprite>();
	justSprite_->Initialize("Textures/UI", "guide_JUST.png");
	justSprite_->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2.0f, 64.0f });
	justSprite_->isActive_ = false;
	PostEffectManager::GetInstance()->AddSpriteList(justSprite_.get());

	// ジャスト回避
	justEvasionSystem_ = std::make_unique<JustEvasionSystem>();
	justEvasionSystem_->Initialize();
	justEvasionSystem_->SetPlayer(this);
	justEvasionSystem_->SetCamera(camera_);

	// ジャスト判定
	justEvasionCollider_ = std::make_unique<Collider>();
	justEvasionCollider_->SetCollisionPrimitive(kCollisionOBB);
	justEvasionCollider_->SetCollisionAttribute(kCollisionAttributePlayer);
	justEvasionCollider_->SetCollisionMask(~kCollisionAttributePlayer);
	justEvasionCollider_->SetOnCollision(std::bind(&Player::JustOnCollision, this, std::placeholders::_1));
	justEvasionCollider_->SetIsActive(true);

	// 被弾時の演出
	hitSystem_ = std::make_unique<HitSystem>();
	hitSystem_->Initialize();
	hitSystem_->SetEndFrame(30.0f);

	// 死亡フラグ
	isDead_ = false;

	// 無敵フラグ
	isInvinsible_ = false;
	// 無敵時間
	invinsibleFrame_ = kMaxInvinsibleFrame;

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "ParticleEmitterPos_RightWing", particles_[0]->emitter_.transform.translate);
	globalVariables->AddItem(groupName, "ParticleEmitterPos_LeftWing", particles_[1]->emitter_.transform.translate);

	particles_[0]->emitter_.transform.translate = globalVariables->GetVector3Value(groupName, "ParticleEmitterPos_RightWing");
	particles_[1]->emitter_.transform.translate = globalVariables->GetVector3Value(groupName, "ParticleEmitterPos_LeftWing");
}

void Player::Update() {
	// 自機が死んでいるなら演出に入る
	DeadAnimation();

	justEvasionSystem_->Update();

	// 自機が死なないと処理しない
	if (isDead_) { return; }

	Move();	  // 移動処理
	Aim();	  // レティクルの配置と移動処理

	// 自機が移動範囲を超えないようにする処理
	object3d_->worldTransform.translate.x = std::clamp<float>(object3d_->worldTransform.translate.x, -kMoveLimit.x, kMoveLimit.x);
	object3d_->worldTransform.translate.y = std::clamp<float>(object3d_->worldTransform.translate.y, -5.0f, kMoveLimit.y);
	object3d_->worldTransform.translate.z = std::clamp<float>(object3d_->worldTransform.translate.z, -kMoveLimit.z, kMoveLimit.z);
	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();

	// z方向にジャスト判定を進める
	justEvasionCollider_->worldTransform.translate.z = GetWorldPosition().z;

	// 無敵状態の更新処理
	InvinsibleUpdate();

	// HPの更新処理
	HPUpdate();

	// 弾ゲージの更新処理
	BulletGaugeUpdate();

	// 自機の軌道パーティクル
	for (int i = 0; i < particles_.size(); i++) {
		particles_[i]->Update();
	}

	// ImGui
	ImGuiParameter();
}

void Player::Draw() {
	// 自機
	object3d_->Draw(playerTexture_);
	// 残像
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		afterImageObject3d_[i]->Draw(playerTexture_);
	}
}

void Player::DrawUI() {
	// 軌道パーティクル
	for (int i = 0; i < particles_.size(); i++) {
		//particles_[i]->Draw(defaultTexture);
	}
	// 死亡パーティクル
	deadParticle_->Draw(deadParticleTexture);
	// ジャスト回避のパーティクル
	justEvasionParticle_->Draw(deadParticleTexture);

	justEvasionSystem_->Draw();
}

void Player::InputUpdate(Vector3& move, Vector3& rotate) {
#pragma region キーボード
	// キーボード入力
	if (input_->PressKey(DIK_UP)) {
		rotate.x -= kMaxRotSpeed.x;
	}
	if (input_->PressKey(DIK_DOWN)) {
		rotate.x += kMaxRotSpeed.x;
	}
	if (input_->PressKey(DIK_RIGHT)) {
		rotate.y += kMaxRotSpeed.y;
		rotate.z -= kMaxRotSpeed.z;
	}
	if (input_->PressKey(DIK_LEFT)) {
		rotate.y -= kMaxRotSpeed.y;
		rotate.z += kMaxRotSpeed.z;
	}
	// ブースト
	if (input_->PressKey(DIK_B)) {
		boost_.isActive = true;
		boostRotAnim_.SetIsStart(true);
		move.z += 1.0f;
	}
	// 回避
	if (input_->TriggerKey(DIK_C)) {
		justEvasionCollider_->worldTransform.translate = GetWorldPosition();
		evasion_.isActive = true;
		evasionRotSpeedAnim_.ResetData();
		evasionRotSpeedAnim_.SetIsStart(true);
	}
#pragma endregion

#pragma region ゲームパッド
	// ゲームパッド状態取得
	if (input_->GetJoystickState(0, joyState_)) {
		// デッドゾーンの設定
		SHORT leftThumbX = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbLX);
		SHORT leftThumbY = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbLY);
		rotate.y += (float)leftThumbX / SHRT_MAX * kMaxRotSpeed.y;
		rotate.z -= (float)leftThumbX / SHRT_MAX * kMaxRotSpeed.z;
		rotate.x -= (float)leftThumbY / SHRT_MAX * kMaxRotSpeed.x;
	}
	// ブースト
	if (input_->GamePadPress(XINPUT_GAMEPAD_X)) {
		boost_.isActive = true;
		boostRotAnim_.SetIsStart(true);
		move.z += 1.0f;
	}
	// 回避
	if (input_->GamePadTrigger(XINPUT_GAMEPAD_A)) {
		justEvasionCollider_->worldTransform.translate = GetWorldPosition();
		evasion_.isActive = true;
		evasionRotSpeedAnim_.ResetData();
		evasionRotSpeedAnim_.SetIsStart(true);
	}
#pragma endregion
}

void Player::Move() {
	// キャラクターの移動ベクトル
	Vector3 move{};
	// 自機の回転
	Vector3 rotate{};
	// 加速モードをOFFにする
	boost_.isActive = false;

	// 入力処理
	// ジャスト回避成功時は操作を受け付けない
	InputUpdate(move, rotate);

#pragma region 指数補間で自機とカメラの回転角を徐々に上げる,下げる
	// 縦移動
	// 移動をしていない場合
	if (rotate.x == 0.0f) {
		// 徐々に速度を落とす
		rotateVel_.x = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.x * 3, 0.1f).x * gameTimer_->GetTimeScale();
		// カメラ演出
		cameraRotateOffset_.x = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.x * gameTimer_->GetTimeScale(), 0.15f).x;
	}
	else {
		// 徐々に速度を上げる
		rotateVel_.x = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.x, 0.1f).x * gameTimer_->GetTimeScale();
		// カメラ演出
		cameraRotateOffset_.x = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.x * gameTimer_->GetTimeScale(), 0.1f).x;
	}
	// 横移動
	// 移動をしていない場合
	if (rotate.y == 0.0f) {
		// 徐々に速度を落とす
		rotateVel_.y = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.y * 4, 0.1f).y * gameTimer_->GetTimeScale();
		rotateVel_.z = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.z * 4, 0.1f).z * gameTimer_->GetTimeScale();
		// カメラ演出
		cameraRotateOffset_.y = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.y * gameTimer_->GetTimeScale(), 0.15f).y;
		cameraRotateOffset_.z = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.z * gameTimer_->GetTimeScale(), 0.15f).z;
	}
	else {
		// 徐々に速度を上げる
		rotateVel_.y = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.y, 0.1f).y * gameTimer_->GetTimeScale();
		rotateVel_.z = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.z, 0.1f).z * gameTimer_->GetTimeScale();
		// カメラ演出
		cameraRotateOffset_.y = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.y * gameTimer_->GetTimeScale(), 0.2f).y;
		cameraRotateOffset_.z = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.z * gameTimer_->GetTimeScale(), 0.2f).z;
	}
#pragma endregion

#pragma region 向いている方向に移動
	Vector3 velocity = { 0.0f, 0.0f, kMaxSpeed };
	// 現在の回転角を取得
	Vector3 rot = object3d_->worldTransform.rotate;
	// 回転行列を求める
	Matrix4x4 rotMatrix = MakeRotateMatrix(rot);
	// 方向ベクトルを求める
	moveVel_ = TransformNormal(velocity, rotMatrix) * 0.8f * gameTimer_->GetTimeScale();
#pragma endregion

	// 加速処理
	BoostUpdate(move.z);
	// 回避処理
	EvasionUpdate(rotate.y, rotate.x);

	// 被弾時の演出
	hitSystem_->Update();

	// 求めた回転を代入
	object3d_->worldTransform.rotate.x = rotateVel_.x + hitSystem_->GetHitRotate().x;
	object3d_->worldTransform.rotate.y = rotateVel_.y + hitSystem_->GetHitRotate().y;
	object3d_->worldTransform.rotate.z = rotateVel_.z + hitSystem_->GetHitRotate().z + boost_.rotVelZ + evasion_.rotVel.z;
	// 速度を自機に加算
	object3d_->worldTransform.translate.x += moveVel_.x;
	object3d_->worldTransform.translate.y += moveVel_.y + evasion_.offset.y;
	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();

	// カメラ移動
	cameraOffset_ += moveVel_ * 0.6f;
	// カメラの移動幅上限
	cameraOffset_.x = std::clamp<float>(cameraOffset_.x, -19.5f, 19.5f);
	cameraOffset_.y = std::clamp<float>(cameraOffset_.y, -5.5f, 5.5f);
	cameraOffset_.z = 0.0f;
}

void Player::Aim() {
}

void Player::Attack() {
	// Rトリガーを押していたら
	//if (joyState_.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER || Input::GetInstance()->PressKey(DIK_SPACE)) {
	//	// 弾ゲージが0以外で発射クールタイムが0のとき
	//	if (bulletInterval_ <= 0 && bulletGauge_.value >= kDecrementBulletGauge) {
	//		//for (LockOnData& lockOnPos : lockOnDatas_) {
	//		//	if (lockOnPos.isActive) { continue; }
	//		//	lockOnPos.isActive = true;

	//			// 弾の速度
	//			const float kBulletSpeed = 5.0f;
	//			Vector3 velocity(0, 0, kBulletSpeed);
	//			// 自機の座標
	//			Vector3 worldPos = GetWorldPosition();

	//			// 速度ベクトルを自機の向きに合わせて回転させる
	//			velocity = TransformNormal(velocity, object3d_->worldTransform.matWorld_);
	//			// 自機から照準オブジェクトへのベクトル
	//			//Vector3 targetPos = *lockOnPos.targetPos;
	//			//velocity = targetPos - worldPos;
	//			//velocity = Normalize(velocity);
	//			//velocity *= kBulletSpeed;

	//						// 自機から照準オブジェクトへのベクトル
	//			Vector3 worldReticlePos = {
	//				object3dReticle_[0]->worldTransform.matWorld_.m[3][0], object3dReticle_[0]->worldTransform.matWorld_.m[3][1],
	//				object3dReticle_[0]->worldTransform.matWorld_.m[3][2] };
	//			velocity = worldReticlePos - worldPos;
	//			velocity = Normalize(velocity);
	//			velocity *= kBulletSpeed;

	//			// 弾を生成し、初期化
	//			PlayerBullet* newBullet = new PlayerBullet();
	//			newBullet->SetCamera(camera_);
	//			//newBullet->Initialize(models_[2], worldPos, velocity, lockOnPos.targetPos);
	//			newBullet->Initialize(models_[2], worldPos, velocity);
	//			// 弾を登録
	//			gameSystem_->AddPlayerBullet(newBullet);

	//			// 弾の発射間隔リセット
	//			bulletInterval_ = kBulletInterval;

	//			// 弾ゲージを減少させる
	//			bulletGauge_.value -= kDecrementBulletGauge;

	//			// 音の再生
	//			audio_->SoundPlayWave(shotSE_, false, 0.25f);

	//			//if (lockOnPos.isActive) { break; }
	//		//}
	//	}
	//}

	//bulletInterval_--;
	//// 0未満にならないようにする
	//bulletInterval_ = std::clamp<int>(bulletInterval_, 0, kBulletInterval);
}

void Player::BoostUpdate(float moveZ) {
	// アニメーションを更新
	boostRotAnim_.Update();

	// 弾ゲージの増加量
	float incrementValue = kIncrementBulletGauge;

	// 加速中なら速度を上げる
	if (boost_.isActive) {
		// 徐々に速度を上げる
		boost_.moveSpeed = Lerps::ExponentialInterpolate(boost_.moveSpeed, moveZ, kMoveSpeedDecayRate.z, 1.0f) * gameTimer_->GetTimeScale();
		// 自機を徐々に前に出す
		object3d_->worldTransform.translate.z = Lerps::ExponentialInterpolate(object3d_->worldTransform.translate.z, 10.0f, kMoveSpeedDecayRate.z, 1.0f);

		// 弾ゲージを上昇量を増加
		incrementValue *= bulletGauge_.magnification;
	}
	else {
		// 徐々に速度を下げる
		boost_.moveSpeed = Lerps::ExponentialInterpolate(boost_.moveSpeed, moveZ, kMoveSpeedDecayRate.z, 1.0f) * gameTimer_->GetTimeScale();
		// 自機をもとの位置徐々に戻す
		object3d_->worldTransform.translate.z = Lerps::ExponentialInterpolate(object3d_->worldTransform.translate.z, 0.0f, kMoveSpeedDecayRate.z, 1.0f);
	}

	// 速度を加算
	moveVel_.z += boost_.moveSpeed * gameTimer_->GetTimeScale();

	// 弾ゲージの増加量を適用
	if (!bulletGauge_.isMax) {
		bulletGauge_.incrementValue = incrementValue;
	}
}

void Player::EvasionUpdate(float rotateY, float rotateX) {
	// ジャスト回避の更新処理
	JustEvasion();

	// 回避時の回転のイージングを更新
	evasionRotSpeedAnim_.Update();

	// 回避中でなければ終了
	if (!evasion_.isActive) {
		// 移動方向に合わせて回避方向を指定
		evasion_.direction.x = Utility::Sign(rotateY);
		evasion_.direction.y = Utility::Sign(-rotateX);
		return;
	}

#pragma region 回避速度の算出 
	// 回避速度のイージング開始
	evasionSpeedAnim_.SetIsStart(true);
	// 回避速度のイージングを更新
	evasionSpeedAnim_.Update();
	// 回避速度を正規化
	evasion_.moveSpeed *= Normalize(evasion_.direction) * 1.3f;
	moveVel_.x += evasion_.moveSpeed.x * gameTimer_->GetTimeScale();
	moveVel_.y += evasion_.moveSpeed.y * 0.5f * gameTimer_->GetTimeScale();
#pragma endregion

#pragma region 残像の処理
	// 残像を出す
	const int division = (int)afterImageObject3d_.size();
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		if ((int)evasion_.curretFrame % division == i) {
			// α値のイージング開始
			evasionAlphaAnims_[i].SetIsStart(true);
			// 残像オブジェクトを表示
			afterImageObject3d_[i]->SetIsActive(true);
			afterImageObject3d_[i]->worldTransform.translate = object3d_->worldTransform.translate;
			afterImageObject3d_[i]->worldTransform.rotate = object3d_->worldTransform.rotate;
		}
	}
	// 残像のα値のアニメーションを更新
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		evasionAlphaAnims_[i].Update();
		afterImageObject3d_[i]->SetAlpha(evasion_.alphas[i]);
	}
#pragma endregion

	// 時間を進める
	evasion_.curretFrame -= gameTimer_->GetTimeScale();
	// ジャスト猶予時間を進める
	evasion_.justFrame--;

	// イージングが終了したら初期化
	if (evasionSpeedAnim_.GetIsEnd()) {
		// 残像を消す
		for (int i = 0; i < afterImageObject3d_.size(); i++) {
			// アニメーション情報初期化
			evasionAlphaAnims_[i].ResetData();
			afterImageObject3d_[i]->SetIsActive(false);
		}
		// アニメーション情報初期化
		evasionSpeedAnim_.ResetData();
		evasion_.curretFrame = kMaxEvasionFrame;
		// ジャスト回避猶予時間
		evasion_.justFrame = kMaxJustEvasionFrame;
		evasion_.isActive = false;
	}
}

void Player::JustEvasion() {
	if (evasion_.justCurrentFrame > 0) {
		evasion_.justCurrentFrame--;
	}
	else if (evasion_.justCurrentFrame <= 0) {
		justSprite_->isActive_ = false;
		evasion_.isJust = false;
		justEvasionSystem_->SetIsActive(false);
	}

	// ジャスト回避ではないならreturn
	if (!evasion_.isJust) { return; }

	// ジャスト時のパーティクル更新
	justEvasionParticle_->Update();

	// ジャスト回避の情報のみ初期化
	if (evasion_.justCurrentFrame <= 0) {
		evasion_.JustDataReset();
		justEvasionParticle_->SetEmitterSpawnLeft(1);
		return;
	}
}

bool Player::IsJustEvasionFrame() {
	// ジャスト回避猶予時間のとき
	if (evasion_.isActive) {
		if (evasion_.justFrame >= 0) {
			return true;
		}
	}
	return false;
}

void Player::OnCollision(Collider* collider) {
	collider;

	// HP減少処理
	DecrementHP();
}

void Player::JustOnCollision(Collider* collider) {
	// ジャスト回避猶予フレーム中でダメージを食らっていないとき
	if (IsJustEvasionFrame() && !isInvinsible_) {
		// ジャスト回避を起動
		evasion_.isJust = true;
		evasion_.justCurrentFrame = kJustEvasionAllFrame;
		// ジャストUIを表示
		justSprite_->isActive_ = true;

		justEvasionSystem_->SetIsActive(true);

		// 無敵状態にする
		invinsibleFrame_ = kMaxInvinsibleFrame;
		isInvinsible_ = true;
	}
}

void Player::JustParticleUpdate(Particle& particle) {
	if (evasion_.justCurrentFrame >= 30) { return; }
	Vector3 distance = particle.transform.translate; // 左端 (x = 0) までの距離
	Vector3 force = /*-gravityStrength*/Multiply(-1.0f, (distance * distance + 1)); // 距離に反比例する力
	Vector3 acceleration = force / 10.0f; // F = ma の式から加速度を計算
	particle.vel += acceleration; // 加速度を速度に反映
}

void Player::InvinsibleUpdate() {
	if (!isInvinsible_) { return; }

	// 3フレームごとに点滅
	if ((int)invinsibleFrame_ % 3 == 0) {
		object3d_->SetColor(Vector4{ 1,1,1,1 });
	}
	else {
		object3d_->SetColor(Vector4{ 1,1,1,0.1f });
	}
	invinsibleFrame_ -= gameTimer_->GetTimeScale();
	invinsibleFrame_ = std::clamp<float>(invinsibleFrame_, 0, kMaxInvinsibleFrame);

	// 無敵時間終了
	if (invinsibleFrame_ <= 0) {
		isInvinsible_ = false;
		object3d_->SetColor(Vector4{ 1,1,1,1 });
	}
}

void Player::HPUpdate() {
	// HPバーの長さ計算
	// 今のHPバーのサイズ = 最大HPの時のバーのサイズ × (今のHP ÷ 最大HP)
	hp_.sprite->SetSizeX(kMaxHPSize.x * (hp_.value / kMaxHp));

	// 30%未満なら赤色にする
	if (hp_.value / kMaxHp <= 30.0f / 100.0f) {
		// 赤色にする
		hp_.sprite->SetColor(Vector4{ 1,0,0,1 });
		// 音をこもらせる
		audio_->soundMuffleValue = -0.9f;
	}
	else {
		// 緑色にする
		hp_.sprite->SetColor(Vector4{ 0,1,0,1 });
		audio_->soundMuffleValue = 0.0f;
	}
}

void Player::DecrementHP() {
	// 今無敵か
	if (!isInvinsible_) {
		if (object3d_->collider->PressOnCollision()) {
			// 被弾演出開始
			hitSystem_->Start();

			// とりあえず固定で30ダメ食らう
			hp_.value -= 30;

			// 死んでいる
			if (hp_.value <= 0.0f) {
				hp_.value = 0.0f;
				isDead_ = true;
			}

			// 1秒無敵
			invinsibleFrame_ = kMaxInvinsibleFrame;
			isInvinsible_ = true;
		}
	}
}

void Player::BulletGaugeUpdate() {
	// 最大になったらマルチロックオン機能を利用可能にする
	// 弾ゲージが減りつつづける
	if (bulletGauge_.value >= kMaxBulletGauge) {
		bulletGauge_.isMax = true;
		bulletGauge_.incrementValue = kDecrementBulletGauge;
	}
	// 弾ゲージが0になったら時はマルチロックオンできない
	if (bulletGauge_.value <= 0) {
		bulletGauge_.isMax = false;
	}

	// 弾ゲージを増加させる
	bulletGauge_.value += bulletGauge_.incrementValue * gameTimer_->GetTimeScale();
	// 0未満にならないようにする
	bulletGauge_.value = std::clamp<float>(bulletGauge_.value, 0, kMaxBulletGauge);

	// 弾ゲージの長さ計算
	bulletGauge_.sprite->SetSizeX(kMaxBulletGaugeSize.x * (bulletGauge_.value / kMaxBulletGauge));
}

void Player::DeadAnimation() {
	// 自機が死なないと処理しない
	if (!isDead_) { return; }

	// 全ての状態初期化
	evasion_.Reset();
	boost_.Reset();
	justSprite_->isActive_ = false;
	// 残像オブジェクトの表示を消す
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		afterImageObject3d_[i]->SetIsActive(false);
	}

	// 死亡パーティクルの更新
	deadParticle_->Update();

	// 死亡演出時間が過ぎたら終了
	if (deadAnim_.currentFrame <= 0) {
		deadAnim_.isEnd = true;
		return;
	}
	// 死亡演出のヒットストップ時間中なら時間を止めて処理を行わない
	if (deadAnim_.currentFrame >= kMaxDeadAnimationFrame - kMaxDeadHitStopFrame && deadAnim_.currentFrame <= kMaxDeadAnimationFrame) {
		gameTimer_->SetTimeScale(0.0f);
	}

	// 死亡アニメーション開始
	deadAnim_.animation.SetIsStart(true);
	// z軸方向に回転させる
	deadAnim_.rotate.z += 0.1f * gameTimer_->GetTimeScale();

	// 向いている方向に移動
	Vector3 velocity = { 0.0f, 0.0f, kMaxSpeed };
	// 現在の回転角を取得
	Vector3 rot = deadAnim_.rotate;
	rot.z = 0.0f;
	// 回転行列を求める
	Matrix4x4 rotMatrix = MakeRotateMatrix(rot);
	// 方向ベクトルを求める
	deadAnim_.velocity = TransformNormal(velocity, rotMatrix) * 0.2f;

	// アニメーション更新
	deadAnim_.animation.Update();

	// 角度を更新
	object3d_->worldTransform.rotate = deadAnim_.rotate;
	// 座標を更新
	object3d_->worldTransform.translate += deadAnim_.velocity * gameTimer_->GetTimeScale();
	object3d_->worldTransform.UpdateMatrix();

	deadAnim_.currentFrame--;
}

void Player::ImGuiParameter() {

#ifdef _DEBUG
	object3d_->ImGuiParameter("Player");

	ImGui::Begin("Player");
	if (ImGui::TreeNode("EvasionData")) {
		ImGui::DragFloat("JustCurrentFrame", &evasion_.justCurrentFrame, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat("CurrentFrame", &evasion_.curretFrame, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat("JustFrame", &evasion_.justFrame, 1.0f, 0.0f, 100.0f);
		ImGui::Checkbox("IsActive", &evasion_.isActive);
		ImGui::Checkbox("IsJust", &evasion_.isJust);
	}
	ImGui::DragFloat("InvinsibleFrame", &invinsibleFrame_, 1.0f, 0.0f, 100.0f);
	// 体力
	ImGui::DragFloat("Hp", &hp_.value, 1.0f, 0.0f, 100.0f);
	ImGui::DragFloat("BulletGauge:Value", &bulletGauge_.value, 1.0f, 0.0f, 100.0f);

	ImGui::End();

	/// jsonによる数値の変更
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	particles_[0]->emitter_.transform.translate = globalVariables->GetVector3Value(groupName, "ParticleEmitterPos_RightWing");
	particles_[1]->emitter_.transform.translate = globalVariables->GetVector3Value(groupName, "ParticleEmitterPos_LeftWing");
	// ボタンを押したらsave
	if (globalVariables->GetInstance()->GetIsSave()) {
		globalVariables->SaveFile("Player");
	}
#endif
}

const Vector3& Player::GetRotation() {
	Vector3 rotate = object3d_->worldTransform.rotate;
	return rotate;
}

const Vector3& Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos = object3d_->worldTransform.translate;
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3d_->worldTransform.matWorld_.m[3][0];
	worldPos.y = object3d_->worldTransform.matWorld_.m[3][1];
	worldPos.z = object3d_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	object3d_->worldTransform.parent_ = parent;
	// 残像のオブジェクトにも適用
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		afterImageObject3d_[i]->worldTransform.parent_ = parent;
	}
}