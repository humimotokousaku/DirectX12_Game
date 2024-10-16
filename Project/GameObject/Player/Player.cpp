#include "Player.h"

#include "AimAssist/AimAssist.h"
#include "CollisionConfig.h"
#include "GameScene.h"
#include "GameTime.h"
#include "GlobalVariables.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "PostEffectManager.h"
#include "TextureManager.h"

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
	// エイムアシスト
	aimAssist_ = AimAssist::GetInstance();

#pragma region 読み込み
	// 射撃SE
	shotSE_ = audio_->SoundLoadWave("Audio/shot.wav");
	// 自機のテクスチャ
	playerTexture_ = TextureManager::GetInstance()->GetSrvIndex("Textures", "Bob_Red.png");
	TextureManager::GetInstance()->LoadTexture("DefaultTexture", "white.png");
	defaultTexture = TextureManager::GetInstance()->GetSrvIndex("DefaultTexture", "white.png");
#pragma endregion

#pragma region 自機モデル作成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetModel(models_[0]);
	object3d_->SetCamera(camera_);
	object3d_->worldTransform.scale = { 0.5f,0.5f,0.5f };
	//object3d_->worldTransform.scale = { 1.0f,1.0f,1.0f };
	object3d_->worldTransform.UpdateMatrix();
	// colliderの設定
	object3d_->collider->SetCollisionPrimitive(kCollisionOBB);
	object3d_->collider->SetCollisionAttribute(kCollisionAttributePlayer);
	object3d_->collider->SetCollisionMask(~kCollisionAttributePlayer);
	object3d_->collider->SetOnCollision(std::bind(&Player::OnCollision, this, std::placeholders::_1));
	object3d_->collider->SetIsActive(true);
#pragma endregion

	// 自機の残像オブジェクトを作成
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		afterImageObject3d_[i] = std::make_unique<Object3D>();
		afterImageObject3d_[i]->Initialize();
		afterImageObject3d_[i]->SetModel(models_[0]);
		afterImageObject3d_[i]->SetCamera(camera_);
		afterImageObject3d_[i]->worldTransform.scale = { 0.5f,0.5f,0.5f };
		afterImageObject3d_[i]->worldTransform.UpdateMatrix();
		// 透明にする
		afterImageObject3d_[i]->SetColor(Vector4{ 0.3f,0.3f,0.3f,0.6f });
		// 表示をしない
		afterImageObject3d_[i]->SetIsActive(false);
	}

#pragma region レティクル
	// 3Dレティクルモデル作成(デバッグ用)
	for (int i = 0; i < 2; i++) {
		object3dReticle_[i] = std::make_unique<Object3D>();
		object3dReticle_[i]->Initialize();
		object3dReticle_[i]->SetModel(models_[1]);
		object3dReticle_[i]->SetCamera(camera_);
	}
	// ロックオンしてない3Dレティクルの座標
	default3dReticle_.Initialize();
	default3dReticle_ = object3dReticle_[0]->worldTransform;

	// 2Dレティクル作成
	sprite2DReticle_[0].Initialize("", "reticle.png");
	sprite2DReticle_[0].SetSize(Vector2{ 50.0f,50.0f });
	sprite2DReticle_[1].Initialize("", "reticle.png");
	// ロックオン時のレティクル作成
	sprite2DReticle_[2].Initialize("", "lockOnReticle.png");
	for (int i = 0; i < 3; i++) {
		sprite2DReticle_[i].SetPos(Vector2((float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2));
		PostEffectManager::GetInstance()->AddSpriteList(&sprite2DReticle_[i]);
	}
#pragma endregion

#pragma region HPのUI作成
	// HPバーのサイズ指定
	hpSize_ = kMaxHPSize;
	hpSprite_.Initialize("DefaultTexture", "white.png");
	hpSprite_.SetSize(hpSize_);
	hpSprite_.SetAnchorPoint(Vector2{ 0.0f,0.5f });
	hpSprite_.SetPos(Vector2(64.0f, 64.0f));
	// 緑色にする
	hpSprite_.SetColor(Vector4{ 0,1,0,1 });
	PostEffectManager::GetInstance()->AddSpriteList(&hpSprite_);
#pragma endregion

#pragma region パーティクル
	// 自機の軌道パーティクルの作成
	for (int i = 0; i < particle_.size(); i++) {
		particle_[i] = std::make_unique<Particles>();
		particle_[i]->Initialize(GetWorldPosition());
		particle_[i]->SetCamera(camera_);
		particle_[i]->SetEmitterParent(&object3d_->worldTransform);
		// 発生頻度
		particle_[i]->SetEmitterFrequency(1.0f / 240.0f);
		// 一度に発生する個数
		particle_[i]->SetEmitterCount(1);
		// ランダムを切る
		particle_[i]->OffRandom();
		// パーティクル一粒の詳細設定
		particle_[i]->particle_.color = { 1,1,1,0.6f };
		particle_[i]->particle_.lifeTime = 1.0f;
		particle_[i]->particle_.transform.translate = { 0.0f,0.0f,0.0f };
		particle_[i]->particle_.transform.scale = { 0.1f,0.1f,0.1f };
		particle_[i]->particle_.vel = { 0.0f,0.0f,0.0f };
	}
#pragma endregion

#pragma region アニメーションの設定
	// ロックオン時のレティクルのアニメーション
	reticleAnim_.SetAnimData(sprite2DReticle_[2].GetSizeP(), Vector2{ 256,256 }, Vector2{ 86,86 }, 8, "LockOnReticle_size_Anim", Easings::EaseInExpo);
	// ロックオン時のレティクルのアニメーション
	boostRotAnim_.SetAnimData(&boostRotVelZ_, 0.0f, float{ 4.0f * M_PI }, 90, "BoostRotAnim", Easings::EaseOutExpo);
	// 回避速度のイージング
	evasionSpeedAnim_.SetAnimData(&evasionSpeed_, kMaxEvasionSpeed, 0.0f, kMaxEvasionFrame, "EvasionSpeedAnim", Easings::EaseOutExpo);
#pragma endregion

	// 加速時の自機のZ軸の回転速度
	boostRotVelZ_ = 0.0f;

	// 死亡フラグ
	isDead_ = false;

	// HP
	hp_ = kMaxHp;

	isInvinsible_ = false;
	// 無敵時間
	invinsibleFrame_ = kMaxInvinsibleFrame;
	// 残像表示時間
	evasionFrame_ = kMaxEvasionFrame;

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "ParticleEmitterPos", particle_[0]->emitter_.transform.translate);
	globalVariables->AddItem(groupName, "ParticleEmitterPos_LeftWing", particle_[1]->emitter_.transform.translate);
	globalVariables->AddItem(groupName, "ParticleEmitterPos_RightWing", particle_[2]->emitter_.transform.translate);
	particle_[0]->emitter_.transform.translate = globalVariables->GetVector3Value(groupName, "ParticleEmitterPos");
	particle_[1]->emitter_.transform.translate = globalVariables->GetVector3Value(groupName, "ParticleEmitterPos_LeftWing");
	particle_[2]->emitter_.transform.translate = globalVariables->GetVector3Value(groupName, "ParticleEmitterPos_RightWing");
}

void Player::Update() {
	Move();	  // 移動処理
	Aim();	  // レティクルの配置と移動処理
	Attack(); // 弾の発射処理

	// 範囲を超えない処理
	object3d_->worldTransform.translate.x = std::clamp<float>(object3d_->worldTransform.translate.x, -kMoveLimit.x, kMoveLimit.x);
	object3d_->worldTransform.translate.y = std::clamp<float>(object3d_->worldTransform.translate.y, -5.0f, kMoveLimit.y);
	object3d_->worldTransform.translate.z = std::clamp<float>(object3d_->worldTransform.translate.z, -kMoveLimit.z, kMoveLimit.z);
	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();

	// 無敵状態の更新処理
	InvinsibleUpdate();

	// HPの更新処理
	HPUpdate();

	// 自機の軌道パーティクル
	for (int i = 0; i < particle_.size(); i++) {
		particle_[i]->Update();
	}

	// 音のこもり具合
	audio_->SetMuffle(shotSE_, 1.0f);

	// ImGui
#ifdef _DEBUG
	object3d_->ImGuiParameter("Player");
	ImGui::Begin("Player");

	if (ImGui::TreeNode("Velocity")) {
		if (ImGui::TreeNode("Translate")) {
			ImGui::DragFloat3("Current", &moveVel_.x, 0);
			ImGui::DragFloat3("Attenuation", &kMoveSpeedDecayRate.x, 0.1f, -10.0f, 10.0f);
			ImGui::DragFloat("Limit", &kMaxSpeed, 0.1f, -10.0f, 10.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Rotate")) {
			ImGui::DragFloat3("Current", &rotateVel_.x, 0);
			ImGui::DragFloat3("Attenuation", &kRotateSpeedDecayRate.x, 0.1f, -10.0f, 10.0f);
			ImGui::DragFloat3("Limit", &kMaxRotSpeed.x, 0.1f, -10.0f, 10.0f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	// レティクルの情報
	object3dReticle_[0]->ImGuiParameter("Reticle0");
	object3dReticle_[1]->ImGuiParameter("Reticle1");
	// 体力
	ImGui::DragFloat("Hp", &hp_, 1.0f, 0.0f, 100.0f);

	ImGui::End();

	/// jsonによる数値の変更
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	particle_[0]->emitter_.transform.translate = globalVariables->GetVector3Value(groupName, "ParticleEmitterPos");
	particle_[1]->emitter_.transform.translate = globalVariables->GetVector3Value(groupName, "ParticleEmitterPos_LeftWing");
	particle_[2]->emitter_.transform.translate = globalVariables->GetVector3Value(groupName, "ParticleEmitterPos_RightWing");
	// ボタンを押したらsave
	if (globalVariables->GetInstance()->GetIsSave()) {
		globalVariables->SaveFile("Player");
	}
#endif
}

void Player::Draw() {
	// 自機
	object3d_->Draw(playerTexture_);
	// 残像
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		afterImageObject3d_[i]->Draw(playerTexture_);
	}

	// 3Dレティクル
#ifdef _DEBUG
	for (int i = 0; i < 2; i++) {
		object3dReticle_[i]->Draw();
	}
#endif // _DEBUG
}

void Player::DrawUI() {
	// ロックオン時のレティクル
	if (*isLockOn_) {
		sprite2DReticle_[2].Draw();
	}

	// 軌道
	for (int i = 1; i < particle_.size(); i++) {
		particle_[i]->Draw(defaultTexture);
	}
}

void Player::BoostUpdate(float moveZ) {
	// アニメーションを更新
	boostRotAnim_.Update();

	// 加速中なら速度を上げる
	if (isBoost_) {
		// 徐々に速度を上げる
		boostSpeed_ = Lerps::ExponentialInterpolate(boostSpeed_, moveZ, kMoveSpeedDecayRate.z, 1.0f);
		// 自機を徐々に前に出す
		object3d_->worldTransform.translate.z = Lerps::ExponentialInterpolate(object3d_->worldTransform.translate.z, 10.0f, kMoveSpeedDecayRate.z, 1.0f);

		// コントローラの振動開始
		input_->GamePadVibration(0, 65535, 65535);
		// アニメーションが終了しているなら振動させない
		if (boostRotAnim_.GetIsEnd()) {
			input_->GamePadVibration(0, 0, 0);
		}
		// ガウスをかける
		PostEffectManager::GetInstance()->radialBlurData_.isActive = true;
	}
	else {
		// 徐々に速度を下げる
		boostSpeed_ = Lerps::ExponentialInterpolate(boostSpeed_, moveZ, kMoveSpeedDecayRate.z, 1.0f);
		// 自機をもとの位置徐々に戻す
		object3d_->worldTransform.translate.z = Lerps::ExponentialInterpolate(object3d_->worldTransform.translate.z, 0.0f, kMoveSpeedDecayRate.z, 1.0f);

		// コントローラの振動除去
		input_->GamePadVibration(0, 0, 0);
		// ガウスを消す
		PostEffectManager::GetInstance()->radialBlurData_.isActive = false;
	}

	// 速度を加算
	moveVel_.z += boostSpeed_;
}

void Player::EvasionUpdate(float moveX) {
	// 回避中でなければ終了
	if (!isEvasion_) {
		if (moveX < 0) { evasionVelX_ = -1; }
		else if (moveX > 0) { evasionVelX_ = 1; }
		return;
	}

	// 残像を出す
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		if (evasionFrame_ % 6 == i) {
			afterImageObject3d_[i]->SetIsActive(true);
			afterImageObject3d_[i]->worldTransform.translate = object3d_->worldTransform.translate;
			afterImageObject3d_[i]->worldTransform.rotate = object3d_->worldTransform.rotate;
		}
	}

	// 回避速度のイージング開始
	evasionSpeedAnim_.SetIsStart(true);

	// 回避速度のイージングを更新
	evasionSpeedAnim_.Update();

	// 速度を正規化
	evasionSpeed_ *= Normalize(evasionVelX_) * 1.2f;
	moveVel_.x = evasionSpeed_;

	// 時間を進める
	evasionFrame_--;

	// イージングが終了したら初期化
	if (evasionSpeedAnim_.GetIsEnd()) {
		evasionSpeedAnim_.ResetData();
		evasionFrame_ = kMaxEvasionFrame;
		isEvasion_ = false;
		// 残像を消す
		for (int i = 0; i < afterImageObject3d_.size(); i++) {
			afterImageObject3d_[i]->SetIsActive(false);
		}
	}
}

void Player::Move() {
	// キャラクターの移動ベクトル
	Vector3 move{};
	// 自機の回転
	Vector3 rotate{};
	// 加速モードをOFFにする
	isBoost_ = false;

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
		isBoost_ = true;
		boostRotAnim_.SetIsStart(true);
		move.z += 1.0f;
	}
	// 回避
	if (input_->PressKey(DIK_C)) {
		isEvasion_ = true;
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
		isBoost_ = true;
		boostRotAnim_.SetIsStart(true);
		move.z += 1.0f;
	}
	// 回避
	if (Input::GetInstance()->GamePadPress(XINPUT_GAMEPAD_A)) {
		isEvasion_ = true;
	}
#pragma endregion

#pragma region 指数補間で自機とカメラの回転角を徐々に上げる,下げる
	// 縦移動
	// 移動をしていない場合
	if (rotate.x == 0.0f) {
		// 徐々に速度を落とす
		rotateVel_.x = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.x * 3, 0.1f).x;
		// カメラ演出
		cameraRotateOffset_.x = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.x, 0.15f).x;
	}
	else {
		// 徐々に速度を上げる
		rotateVel_.x = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.x, 0.1f).x;
		// カメラ演出
		cameraRotateOffset_.x = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.x, 0.1f).x;
	}
	// 横移動
	// 移動をしていない場合
	if (rotate.y == 0.0f) {
		// 徐々に速度を落とす
		rotateVel_.y = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.y * 3, 0.1f).y;
		rotateVel_.z = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.z * 3, 0.1f).z;
		// カメラ演出
		cameraRotateOffset_.y = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.y, 0.15f).y;
		cameraRotateOffset_.z = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.z, 0.15f).z;
	}
	else {
		// 徐々に速度を上げる
		rotateVel_.y = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.y, 0.1f).y;
		rotateVel_.z = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedDecayRate.z, 0.1f).z;
		// カメラ演出
		cameraRotateOffset_.y = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.y, 0.1f).y;
		cameraRotateOffset_.z = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedDecayRate.z, 0.1f).z;
	}
#pragma endregion



	// 向いている方向に移動
	Vector3 velocity = { 0, 0, kMaxSpeed };
	// 現在の回転角を取得
	Vector3 rot = object3d_->worldTransform.rotate;
	// 回転行列を求める
	Matrix4x4 rotMatrix = MakeRotateMatrix(rot);
	// 方向ベクトルを求める
	moveVel_ = TransformNormal(velocity, rotMatrix) * 0.8f;


	// 移動速度の上限
	moveVel_.x = std::clamp<float>(moveVel_.x, -kMaxSpeed, kMaxSpeed);
	moveVel_.y = std::clamp<float>(moveVel_.y, -kMaxSpeed, kMaxSpeed);
	// 回転速度の上限
	rotateVel_.x = std::clamp<float>(rotateVel_.x, -kMaxRotSpeed.x, kMaxRotSpeed.x);
	rotateVel_.y = std::clamp<float>(rotateVel_.y, -kMaxRotSpeed.y, kMaxRotSpeed.y);
	rotateVel_.z = std::clamp<float>(rotateVel_.z, -kMaxRotSpeed.z, kMaxRotSpeed.z);

	// 加速処理
	BoostUpdate(move.z);

	// 回避処理
	EvasionUpdate(rotate.y);

	// 求めた回転を代入
	object3d_->worldTransform.rotate.x = rotateVel_.x;
	object3d_->worldTransform.rotate.y = rotateVel_.y;
	object3d_->worldTransform.rotate.z = rotateVel_.z + boostRotVelZ_;
	// 速度を自機に加算
	object3d_->worldTransform.translate.x += moveVel_.x;
	object3d_->worldTransform.translate.y += moveVel_.y;

	// カメラ移動
	cameraOffset_ += moveVel_ / 5;
	// カメラの移動幅上限
	cameraOffset_.x = std::clamp<float>(cameraOffset_.x, -3.5f, 3.5f);
	cameraOffset_.y = std::clamp<float>(cameraOffset_.y, -3.5f, 3.5f);
	cameraOffset_.z = 0.0f;

	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();
}

void Player::Aim() {
	// 3Dレティクル配置
	Deploy3DReticle();

	// ロックオン処理(必ず3Dレティクル配置と2Dレティクル配置の間に書く)
	aimAssist_->LockOn();

	// ロックオンしたら赤くなる
	if (*isLockOn_) {
		// 補間量を足す
		object3dReticle_[0]->worldTransform.translate += *lockOnReticleOffset_;
		sprite2DReticle_[0].SetColor(Vector4{ 1,0,0,1 });
		// ロックオンレティクルの配置と挙動
		DeployLockOnReticle();
	}
	else {
		object3dReticle_[0]->worldTransform.translate += *lockOnReticleOffset_;
		sprite2DReticle_[0].SetColor(Vector4{ 1,1,1,1 });
		sprite2DReticle_[2].SetSize(sprite2DReticle_[2].GetStartingSize());
		sprite2DReticle_[2].SetRotate(Vector3{ 0,0,0 });
		reticleAnim_.SetIsStart(false);
	}
	sprite2DReticle_[2].isActive_ = *isLockOn_;
	reticleAnim_.Update();

	// 2Dレティクル配置
	Deploy2DReticle();
}

void Player::Attack() {
	// Rトリガーを押していたら
	if (joyState_.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER || Input::GetInstance()->PressKey(DIK_SPACE)) {
		if (bulletInterval_ <= 0) {
			// 弾の速度
			const float kBulletSpeed = 5.0f;
			Vector3 velocity(0, 0, kBulletSpeed);
			// 速度ベクトルを自機の向きに合わせて回転させる
			Vector3 worldPos = GetWorldPosition();
			velocity = TransformNormal(velocity, object3d_->worldTransform.matWorld_);
			// 自機から照準オブジェクトへのベクトル
			Vector3 worldReticlePos = {
				object3dReticle_[0]->worldTransform.matWorld_.m[3][0], object3dReticle_[0]->worldTransform.matWorld_.m[3][1],
				object3dReticle_[0]->worldTransform.matWorld_.m[3][2] };
			velocity = worldReticlePos - worldPos;
			velocity = Normalize(velocity);
			velocity *= kBulletSpeed;

			// 弾を生成し、初期化
			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->SetCamera(camera_);
			newBullet->Initialize(models_[2], worldPos, velocity);
			// 弾を登録
			gameScene_->AddPlayerBullet(newBullet);

			// 弾の発射間隔リセット
			bulletInterval_ = kBulletInterval;

			// 音の再生
			audio_->SoundPlayWave(shotSE_, false, 0.25f);
		}
	}

	bulletInterval_--;
	// 0未満にならないようにする
	bulletInterval_ = std::clamp<int>(bulletInterval_, 0, kBulletInterval);
}

void Player::OnCollision(Collider* collider) {
	// HP減少処理
	DecrementHP();
}

void Player::InvinsibleUpdate() {
	if (!isInvinsible_) { return; }

	// 3フレームごとに点滅
	if (invinsibleFrame_ % 3 == 0) {
		object3d_->SetColor(Vector4{ 1,1,1,1 });
	}
	else {
		object3d_->SetColor(Vector4{ 1,1,1,0.1f });
	}
	invinsibleFrame_--;
	invinsibleFrame_ = std::clamp<int>(invinsibleFrame_, 0, kMaxInvinsibleFrame);

	// 無敵時間終了
	if (invinsibleFrame_ <= 0) {
		isInvinsible_ = false;
		object3d_->SetColor(Vector4{ 1,1,1,1 });
	}
}

void Player::HPUpdate() {
	// HPバーの長さ計算
	// 今のHPバーのサイズ = 最大HPの時のバーのサイズ × (今のHP ÷ 最大HP)
	hpSprite_.SetSizeX(kMaxHPSize.x * (hp_ / kMaxHp));

	// 30%未満なら赤色にする
	if (hp_ / kMaxHp <= 30.0f / 100.0f) {
		// 赤色にする
		hpSprite_.SetColor(Vector4{ 1,0,0,1 });
		// 音をこもらせる
		audio_->soundMuffleValue = -0.9f;
	}
	else {
		// 緑色にする
		hpSprite_.SetColor(Vector4{ 0,1,0,1 });
		audio_->soundMuffleValue = 0.0f;
	}
}

void Player::DecrementHP() {
	// 今無敵か
	if (!isInvinsible_) {
		if (object3d_->collider->PressOnCollision()) {
			// とりあえず固定で30ダメ食らう
			hp_ -= 30;
			// 死んでいる
			if (hp_ <= 0.0f) {
				hp_ = 0.0f;
				isDead_ = true;
			}

			// 1秒無敵
			invinsibleFrame_ = kMaxInvinsibleFrame;
			isInvinsible_ = true;
		}
	}
}

void Player::DeadAnimation() {
	if (isDead_) {

	}
}

void Player::Deploy3DReticle() {
	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset{ 0, 0, kDistanceObject };
	Vector3 rotate = object3d_->worldTransform.rotate + object3d_->worldTransform.parent_->rotate;
	rotate.z = 0;
	// 回転行列を合成
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);
	// 自機のワールド行列の回転を反映する
	offset = TransformNormal(offset, rotateMatrix);
	// 3Dレティクルの座標を設定
	object3dReticle_[0]->worldTransform.translate = GetWorldPosition() + offset;
	object3dReticle_[0]->worldTransform.UpdateMatrix();

	// 自機から3Dレティクルへのオフセット(Z+向き)
	offset = { 0, 0, kDistanceObject / 3 * 2 };
	// 自機のワールド行列の回転を反映する
	offset = TransformNormal(offset, rotateMatrix);
	// 真ん中の3Dレティクルの座標を設定
	object3dReticle_[1]->worldTransform.translate = GetWorldPosition() + offset;
	object3dReticle_[1]->worldTransform.UpdateMatrix();

	// 自機から3Dレティクルへのオフセット(Z+向き)
	offset = { 0, 0, kDistanceObject };
	// 自機のワールド行列の回転を反映する
	offset = TransformNormal(offset, rotateMatrix);
	// 真ん中の3Dレティクルの座標を設定
	default3dReticle_.translate = GetWorldPosition() + offset;
	default3dReticle_.UpdateMatrix();
}

void Player::Deploy2DReticle() {
	// レティクルの座標を更新
	for (int i = 0; i < 2; i++) {
		object3dReticle_[i]->worldTransform.UpdateMatrix();
	}

	// 3Dレティクルのワールド座標を取得
	Vector3 positionReticle = GetWorld3DReticlePosition(0);
	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport{};
	matViewProjectionViewport =
		Multiply(camera_->GetViewProjection().matView, Multiply(camera_->GetViewProjection().matProjection, matViewport));
	// ワールド→スクリーン座標変換
	positionReticle = Transforms(positionReticle, matViewProjectionViewport);
	// スプライトのレティクルに座標を設定
	sprite2DReticle_[0].SetPos(Vector2(positionReticle.x, positionReticle.y));

	// 真ん中のレティクル
	// ワールド→スクリーン座標変換
	positionReticle = GetWorld3DReticlePosition(1);
	positionReticle = Transforms(positionReticle, matViewProjectionViewport);
	// スプライトのレティクルに座標設定
	sprite2DReticle_[1].SetPos(Vector2(positionReticle.x, positionReticle.y));
}

void Player::DeployLockOnReticle() {
	// ロックオン時のレティクルの座標を設定
	sprite2DReticle_[2].SetPos(sprite2DReticle_[0].GetPos());

	// 回転させる
	Vector3 rotate = {
		0.0f,0.0f,0.03f
	};
	sprite2DReticle_[2].SetRotate(sprite2DReticle_[2].GetRotate() + rotate);

	reticleAnim_.SetIsStart(true);
}

Vector3 Player::GetRotation() {
	Vector3 rotate = object3d_->worldTransform.rotate;
	return rotate;
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos = object3d_->worldTransform.translate;
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3d_->worldTransform.matWorld_.m[3][0];
	worldPos.y = object3d_->worldTransform.matWorld_.m[3][1];
	worldPos.z = object3d_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}

Vector3 Player::GetWorld3DReticlePosition(int index) {
	// ワールド座標を入れる変数
	Vector3 worldPos = object3dReticle_[index]->worldTransform.translate;
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3dReticle_[index]->worldTransform.matWorld_.m[3][0];
	worldPos.y = object3dReticle_[index]->worldTransform.matWorld_.m[3][1];
	worldPos.z = object3dReticle_[index]->worldTransform.matWorld_.m[3][2];

	return worldPos;
}

Vector3 Player::GetDefault3DReticlePosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos = default3dReticle_.translate;
	// ワールド行列の平行移動成分を取得
	worldPos.x = default3dReticle_.matWorld_.m[3][0];
	worldPos.y = default3dReticle_.matWorld_.m[3][1];
	worldPos.z = default3dReticle_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	object3d_->worldTransform.parent_ = parent;
	for (int i = 0; i < afterImageObject3d_.size(); i++) {
		afterImageObject3d_[i]->worldTransform.parent_ = parent;
	}
}