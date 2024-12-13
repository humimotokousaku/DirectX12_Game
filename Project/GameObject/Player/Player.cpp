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
	hp_.sprite->isActive_ = false;
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
	bulletGauge_.sprite->isActive_ = false;
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
#pragma endregion

#pragma region アニメーションの設定
	// ブースト時の自機回転アニメーション
	boostRotAnim_.SetAnimData(&boost_.rotVelZ, 0.0f, float{ 4.0f * M_PI }, 90, Easings::EaseOutExpo);

	// 死亡アニメーション
	// 自機を下に向ける
	deadAnim_.animation.SetAnimData(&deadAnim_.rotate.x, 0.0f, (float)std::numbers::pi / 2.0f, 120, Easings::EaseInQuart);

	// タイトルシーンからゲームシーンになるまでのアニメーション
	titleAnim_.SetAnimData(&object3d_->worldTransform.rotate, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ -(float)std::numbers::pi / 10.0f, 0.0f, (float)std::numbers::pi * 2.0f }, 60, Easings::EaseOutExpo);
	titleAnim_.SetAnimData(&object3d_->worldTransform.rotate, Vector3{ -(float)std::numbers::pi / 10.0f, 0.0f, 0.0f }, Vector3{ -(float)std::numbers::pi / 4.0f, 0.0f, 0.0f }, 60, Easings::EaseInExpo);
	// クリア時のアニメーション
	clearAnim_.SetAnimData(&object3d_->worldTransform.rotate, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ -(float)std::numbers::pi / 4.0f, -(float)std::numbers::pi / 3.0f, (float)std::numbers::pi / 10.0f }, 60, Easings::EaseInExpo);
	clearAnim_.SetAnimData(&object3d_->worldTransform.rotate, Vector3{ -(float)std::numbers::pi / 10.0f, -(float)std::numbers::pi / 3.0f, (float)std::numbers::pi / 10.0f }, Vector3{ -(float)std::numbers::pi / 5.0f, -(float)std::numbers::pi / 2.0f, (float)std::numbers::pi / 10.0f }, 60, Easings::EaseOutExpo);
#pragma endregion

	// 回避システム
	evasionSystem_ = std::make_unique<EvasionSystem>();
	evasionSystem_->Initialize(this, camera_, models_[0]);

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
	// 移動処理
	Move();	  

	// 自機の移動制限
	MoveLimit();

	// ジャスト判定を進める
	evasionSystem_->JustColliderUpdate();

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

	// 軌道パーティクル
	for (int i = 0; i < particles_.size(); i++) {
		//particles_[i]->Draw(defaultTexture);
	}
	// 死亡パーティクル
	deadParticle_->Draw(deadParticleTexture);

	// DrawUI関数が呼ばれていないときはゲージ系を表示しないようにする
	hp_.sprite->isActive_ = false;
	bulletGauge_.sprite->isActive_ = false;
}

void Player::DrawUI() {
	// HPゲージ
	hp_.sprite->isActive_ = true;
	// 弾ゲージ
	bulletGauge_.sprite->isActive_ = true;

	// 回避時のエフェクト描画
	evasionSystem_->Draw();

	evasionSystem_->DrawUI();
}

void Player::TitleEffect(bool& isEnd) {
	titleAnim_.SetIsStart(true);
	titleAnim_.Update();

#pragma region 向いている方向に移動
	Vector3 velocity = { 0.0f, 0.0f, kMaxSpeed };
	// 現在の回転角を取得(z軸は0にして見た目のみ回転させる)
	Vector3 rot = object3d_->worldTransform.rotate;
	rot.z = 0.0f;
	// 回転行列を求める
	Matrix4x4 rotMatrix = MakeRotateMatrix(rot);
	// 方向ベクトルを求める
	moveVel_ = TransformNormal(velocity, rotMatrix) * 5.0f * gameTimer_->GetTimeScale();
#pragma endregion

	// 速度を自機に加算
	object3d_->worldTransform.translate += moveVel_;
	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();

	// アニメーションが終わり次第タイトル演出を終了する
	if (titleAnim_.GetIsEnd()) {
		isEnd = true;
	}
}

void Player::ClearEffect(bool& isEnd) {
	clearAnim_.SetIsStart(true);
	clearAnim_.Update();

#pragma region 向いている方向に移動
	Vector3 velocity = { 0.0f, 0.0f, kMaxSpeed };
	// 現在の回転角を取得(z軸は0にして見た目のみ回転させる)
	Vector3 rot = object3d_->worldTransform.rotate;
	// 回転行列を求める
	Matrix4x4 rotMatrix = MakeRotateMatrix(rot);
	// 方向ベクトルを求める
	moveVel_ = TransformNormal(velocity, rotMatrix) * 4.0f * gameTimer_->GetTimeScale();
#pragma endregion

	// 速度を自機に加算
	object3d_->worldTransform.translate += moveVel_;
	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();

	// クリアアニメーションが終わったら終了
	if (clearAnim_.GetIsEnd()) {
		isEnd = true;
	}
}

void Player::DeadEffect(bool& isEnd) {
	// 自機が死なないと処理しない
	if (!isDead_) { return; }

	// 全ての状態初期化
	evasionSystem_->Reset();
	boost_.Reset();

	// 死亡パーティクルの更新
	deadParticle_->Update();

	// 死亡演出時間が過ぎたら終了
	if (deadAnim_.currentFrame <= 0) {
		deadAnim_.isEnd = true;
		isEnd = true;
		return;
	}
	// 死亡演出のヒットストップ時間中なら時間を止めて処理を行わない
	if (deadAnim_.currentFrame >= kMaxDeadAnimationFrame - kMaxDeadHitStopFrame && deadAnim_.currentFrame <= kMaxDeadAnimationFrame) {
		gameTimer_->SetTimeScale(0.0f);
	}
	else {
		gameTimer_->SetTimeScale(1.0f);
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
		evasionSystem_->SetJustParameter();
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
		evasionSystem_->SetJustParameter();
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
		rotateVel_.x = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.x * 0.3f).x * gameTimer_->GetTimeScale();
		// カメラ演出
		cameraRotateOffset_.x = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.x * 0.15f * gameTimer_->GetTimeScale()).x;
	}
	else {
		// 徐々に速度を上げる
		rotateVel_.x = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.x * 0.2f).x * gameTimer_->GetTimeScale();
		// カメラ演出
		cameraRotateOffset_.x = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.x * 0.1f * gameTimer_->GetTimeScale()).x;
	}
	// 横移動
	// 移動をしていない場合
	if (rotate.y == 0.0f) {
		// 徐々に速度を落とす
		rotateVel_.y = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.y * 0.4f).y * gameTimer_->GetTimeScale();
		rotateVel_.z = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.z * 0.4f).z * gameTimer_->GetTimeScale();
		// カメラ演出
		cameraRotateOffset_.y = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.y * 0.075f * gameTimer_->GetTimeScale()).y;
		cameraRotateOffset_.z = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.z * 0.075f * gameTimer_->GetTimeScale()).z;
	}
	else {
		// 徐々に速度を上げる
		rotateVel_.y = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.y * 0.2f).y * gameTimer_->GetTimeScale();
		rotateVel_.z = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.z * 0.2f).z * gameTimer_->GetTimeScale();
		// カメラ演出
		cameraRotateOffset_.y = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.y * 0.1f * gameTimer_->GetTimeScale()).y;
		cameraRotateOffset_.z = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.z * 0.1f * gameTimer_->GetTimeScale()).z;
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
	object3d_->worldTransform.rotate.z = rotateVel_.z + hitSystem_->GetHitRotate().z + boost_.rotVelZ + evasionSystem_->GetEvasionData().rotVel.z;
	// 速度を自機に加算
	object3d_->worldTransform.translate.x += moveVel_.x;
	object3d_->worldTransform.translate.y += moveVel_.y + evasionSystem_->GetEvasionData().offset.y;
	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();

	// カメラ移動
	cameraOffset_ += moveVel_ * 0.6f;
	// カメラの移動幅上限
	cameraOffset_.x = std::clamp<float>(cameraOffset_.x, -19.5f, 19.5f);
	cameraOffset_.y = std::clamp<float>(cameraOffset_.y, -5.5f, 5.5f);
	cameraOffset_.z = 0.0f;
}

void Player::MoveLimit() {
	// 自機が移動範囲を超えないようにする処理
	object3d_->worldTransform.translate.x = std::clamp<float>(object3d_->worldTransform.translate.x, -kMoveLimit.x, kMoveLimit.x);
	object3d_->worldTransform.translate.y = std::clamp<float>(object3d_->worldTransform.translate.y, -5.0f, kMoveLimit.y);
	object3d_->worldTransform.translate.z = std::clamp<float>(object3d_->worldTransform.translate.z, -kMoveLimit.z, kMoveLimit.z);
	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();
}

void Player::BoostUpdate(float moveZ) {
	// アニメーションを更新
	boostRotAnim_.Update();

	// 弾ゲージの増加量
	float incrementValue = kIncrementBulletGauge;

	// 加速中なら速度を上げる
	if (boost_.isActive) {
		// 徐々に速度を上げる
		boost_.moveSpeed = Lerps::ExponentialInterpolate(boost_.moveSpeed, moveZ, kMoveSpeedDecayRate.z) * gameTimer_->GetTimeScale();
		// 自機を徐々に前に出す
		object3d_->worldTransform.translate.z = Lerps::ExponentialInterpolate(object3d_->worldTransform.translate.z, 10.0f, kMoveSpeedDecayRate.z);

		// 弾ゲージを上昇量を増加
		incrementValue *= bulletGauge_.magnification;
	}
	else {
		// 徐々に速度を下げる
		boost_.moveSpeed = Lerps::ExponentialInterpolate(boost_.moveSpeed, moveZ, kMoveSpeedDecayRate.z) * gameTimer_->GetTimeScale();
		// 自機をもとの位置徐々に戻す
		object3d_->worldTransform.translate.z = Lerps::ExponentialInterpolate(object3d_->worldTransform.translate.z, 0.0f, kMoveSpeedDecayRate.z);
	}

	// 速度を加算
	moveVel_.z += boost_.moveSpeed * gameTimer_->GetTimeScale();

	// 弾ゲージの増加量を適用
	if (!bulletGauge_.isMax) {
		bulletGauge_.incrementValue = incrementValue;
	}
}

void Player::EvasionUpdate(float rotateY, float rotateX) {
	evasionSystem_->Update(rotateY, rotateX, moveVel_);
}

void Player::OnCollision(Collider* collider) {
	// 衝突した相手がジャスト回避の判定なら処理しない
	if (collider->GetCollisionAttribute() == kCollisionAttributeJustEvasion) { return; }

	// ダメージを食らっていないとき
	if (isInvinsible_ || evasionSystem_->GetIsInvisible()) { return; }
	// ジャスト回避の判定を消す
	evasionSystem_->SetIsActiveJustCollider(false);

	// HP減少処理
	DecrementHP(collider);
}

void Player::InvinsibleUpdate() {
	if (!isInvinsible_) { return; }
	invinsibleFrame_ -= gameTimer_->GetTimeScale();
	invinsibleFrame_ = std::clamp<float>(invinsibleFrame_, 0, kMaxInvinsibleFrame);

	// 無敵時間終了
	if (invinsibleFrame_ <= 0) {
		isInvinsible_ = false;
		object3d_->SetColor(Vector4{ 1,1,1,1 });
		// ジャスト回避可能にする
		evasionSystem_->SetIsActiveJustCollider(true);
	}

	// 3フレームごとに点滅
	if ((int)invinsibleFrame_ % 3 == 0) {
		object3d_->SetColor(Vector4{ 1,1,1,1 });
	}
	else {
		object3d_->SetColor(Vector4{ 1,1,1,0.1f });
	}
}

void Player::HPUpdate() {
	// HPバーの長さ計算
	// 今のHPバーのサイズ = 最大HPの時のバーのサイズ × (今のHP ÷ 最大HP)
	hp_.sprite->SetSizeX(kMaxHPSize.x * (hp_.value / kMaxHp));

	// 30%未満なら赤色にする
	if (hp_.value / kMaxHp <= kLowHp / kMaxHp) {
		// 赤色にする
		hp_.sprite->SetColor(Vector4{ 1,0,0,1 });
		// 音をこもらせる
		//audio_->soundMuffleValue = -0.9f;
	}
	else {
		// 緑色にする
		hp_.sprite->SetColor(Vector4{ 0,1,0,1 });
		//audio_->soundMuffleValue = 0.0f;
	}
}

void Player::DecrementHP(Collider* collider) {
	// 今無敵か
	if (!isInvinsible_) {
		if (object3d_->collider->PressOnCollision()) {
			// 被弾演出開始
			hitSystem_->Start();

			// とりあえず固定で30ダメ食らう
			hp_.value -= collider->GetDamage();

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

void Player::ImGuiParameter() {
#ifdef _DEBUG
	//object3d_->ImGuiParameter("Player");

	ImGui::Begin("Player");
	/*if (ImGui::TreeNode("EvasionData")) {
		ImGui::DragFloat3("JustSize", &justSize_.x, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("JustExtensionFrame", &justFrame_, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat("JustCurrentFrame", &evasion_.justCurrentFrame, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat("CurrentFrame", &evasion_.curretFrame, 1.0f, 0.0f, 100.0f);
		ImGui::DragFloat("JustFrame", &evasion_.justFrame, 1.0f, 0.0f, 100.0f);
		ImGui::Checkbox("IsActive", &evasion_.isActive);
		ImGui::Checkbox("IsJust", &evasion_.isJust);
		ImGui::TreePop();
	}*/
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
	const Vector3& rotate = object3d_->worldTransform.rotate;
	return rotate;
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPosition{};
	// ワールド行列の平行移動成分を取得
	worldPosition.x = object3d_->worldTransform.matWorld_.m[3][0];
	worldPosition.y = object3d_->worldTransform.matWorld_.m[3][1];
	worldPosition.z = object3d_->worldTransform.matWorld_.m[3][2];

	return worldPosition;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	object3d_->worldTransform.parent_ = parent;

	// 残像オブジェクトにも親子関係を結ぶ
	evasionSystem_->SetParent(parent);
}