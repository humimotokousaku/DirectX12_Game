#include "Player.h"
#include "TextureManager.h"
#include "CollisionConfig.h"
#include "GameTime.h"
#include "GameScene.h"
#include "AimAssist/AimAssist.h"
#include "ImGuiManager.h"
#include "Input.h"
#include <numbers>

Player::Player() {}
Player::~Player() {
	models_.clear();
	for (Sprite* sprite : sprite2DReticle_) {
		delete sprite;
	}
	delete hpSprite_;
}

void Player::Initialize() {
	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// 自機モデル作成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetModel(models_[0]);
	object3d_->SetCamera(camera_);
	object3d_->worldTransform.scale = { 0.5f,0.5f,0.5f };
	// 自機のテクスチャ
	playerTexture_ = TextureManager::GetInstance()->GetSrvIndex("Textures", "Bob_Red.png");

	// 3Dレティクルモデル作成
	for (int i = 0; i < 2; i++) {
		object3dReticle_[i] = std::make_unique<Object3D>();
		object3dReticle_[i]->Initialize();
		object3dReticle_[i]->SetModel(models_[1]);
		object3dReticle_[i]->SetCamera(camera_);
	}
	// ロックオンしてない3Dレティクルの座標
	default3dReticle_.Initialize();
	default3dReticle_ = object3dReticle_[0]->worldTransform;

	gameSpeed_ = 1.0f;

	// 2Dレティクル作成
	for (int i = 0; i < 2; i++) {
		sprite2DReticle_[i] = new Sprite();
		sprite2DReticle_[i]->Initialize("", "reticle.png");
		sprite2DReticle_[i]->SetPos(Vector2((float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2));
	}
	sprite2DReticle_[0]->SetSize(Vector2{ 50.0f,50.0f });

	// HP作成
	hpSize_ = kMaxHPSize;
	hpSprite_ = new Sprite();
	hpSprite_->Initialize("DefaultTexture", "white.png");
	hpSprite_->SetSize(hpSize_);
	hpSprite_->SetAnchorPoint(Vector2{ 0.0f,0.5f });
	hpSprite_->SetPos(Vector2(64.0f, 64.0f));
	// 緑色にする
	hpSprite_->SetColor(Vector4{ 0,1,0,1 });
	isInvinsible_ = false;
	// 無敵時間
	invinsibleFrame_ = kMaxInvinsibleFrame;

	// 自機の軌道パーティクルの作成
	particle_ = std::make_unique<Particles>();
	particle_->Initialize();
	particle_->SetCamera(camera_);
	// 発生頻度
	particle_->SetEmitterFrequency(1.0f / 60.0f);
	// 一度に発生する個数
	particle_->SetEmitterCount(1);
	particle_->SetRandomPerticle(false);
	TextureManager::GetInstance()->LoadTexture("DefaultTexture", "white.png");
	defaultTexture = TextureManager::GetInstance()->GetSrvIndex("DefaultTexture", "white.png");

	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
	collisionManager_->SetColliderList(this);

	aimAssist_ = AimAssist::GetInstance();
}

void Player::Update() {
	// 移動処理
	Move();
	// レティクルの配置と移動処理
	Aim();
	// 弾の発射処理
	Attack();

	// 範囲を超えない処理
	object3d_->worldTransform.translate.x = std::clamp<float>(object3d_->worldTransform.translate.x, -kMoveLimit.x, kMoveLimit.x);
	object3d_->worldTransform.translate.y = std::clamp<float>(object3d_->worldTransform.translate.y, -kMoveLimit.y, kMoveLimit.y);
	object3d_->worldTransform.translate.z = std::clamp<float>(object3d_->worldTransform.translate.z, -kMoveLimit.z, kMoveLimit.z);

	// 全ての処理が終わってからz軸を代入
	// 現状、オイラー角を使用しているのでジンバルロックを回避できない。なのでレティクルの挙動をマシにするために最後に入れている
	object3d_->worldTransform.rotate.z = rotateVel_.z;

	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();

	// 無敵演出
	if (isInvinsible_) {
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

	// 自機の軌道パーティクル
	particle_->SetEmitterPos(GetWorldPosition());
	particle_->Update();

#ifdef _DEBUG
	// ImGui
	object3d_->ImGuiParameter("Player");
	object3dReticle_[0]->ImGuiParameter("Reticle0");
	object3dReticle_[1]->ImGuiParameter("Reticle1");
	ImGui::DragFloat3("moveVel", &moveVel_.x, 0);
	ImGui::DragFloat3("rotateVel", &rotateVel_.x, 0);
	//ImGui::DragFloat3("rotateRate", &kRotateSpeedRate.x, 0.001f, 0, 1);
	ImGui::DragFloat3("CameraOffset", &cameraOffset_.x, 0.001f, 0);
#endif
}

void Player::Draw() {
	// 自機
	object3d_->Draw(playerTexture_);
	// 3Dレティクル
#ifdef _DEBUG
	for (int i = 0; i < 2; i++) {
		object3dReticle_[i]->Draw();
	}
#endif // _DEBUG
}

void Player::DrawUI() {
	// レティクル
	for (Sprite* sprite : sprite2DReticle_) {
		sprite->Draw();
	}

	// HPバー
	hpSprite_->Draw();

	// 軌道
	particle_->Draw(defaultTexture);
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	object3d_->worldTransform.parent_ = parent;
}

void Player::OnCollision(Collider* collider) {
	// HP減少処理
	DecrementHP();
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
		move.y += kMaxSpeed;
		rotate.x -= kMaxRotSpeed.x;
	}
	if (input_->PressKey(DIK_DOWN)) {
		move.y -= kMaxSpeed;
		rotate.x += kMaxRotSpeed.x;
	}
	if (input_->PressKey(DIK_RIGHT)) {
		move.x += kMaxSpeed;
		rotate.y += kMaxRotSpeed.y;
		rotate.z -= kMaxRotSpeed.z;
	}
	if (input_->PressKey(DIK_LEFT)) {
		move.x -= kMaxSpeed;
		rotate.y -= kMaxRotSpeed.y;
		rotate.z += kMaxRotSpeed.z;
	}
	// ブースト
	if (input_->PressKey(DIK_B)) {
		isBoost_ = true;
		move.z += 1.0f;
	}
#pragma endregion

#pragma region ゲームパッド
	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState_)) {
		// デッドゾーンの設定
		SHORT leftThumbX = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbLX);
		SHORT leftThumbY = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbLY);
		move.x += (float)leftThumbX / SHRT_MAX * kMaxSpeed;
		move.y += (float)leftThumbY / SHRT_MAX * kMaxSpeed;
		rotate.y += (float)leftThumbX / SHRT_MAX * kMaxRotSpeed.y;
		rotate.z -= (float)leftThumbX / SHRT_MAX * kMaxRotSpeed.z;
		rotate.x -= (float)leftThumbY / SHRT_MAX * kMaxRotSpeed.x;
	}

	// ブースト
	if (Input::GetInstance()->GamePadPress(XINPUT_GAMEPAD_X)) {
		isBoost_ = true;
		move.z += 1.0f;
	}
#pragma endregion

#pragma region 指数補間で速度を徐々に上げる,下げる
	// 縦移動
	// 移動をしていない場合
	if (move.y == 0.0f) {
		// 徐々に速度を落とす
		moveVel_.y = Lerps::ExponentialInterpolate(moveVel_, move, kMoveSpeedAttenuationRate.y, 0.1f).y;
		rotateVel_.x = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedAttenuationRate.x, 0.1f).x;
		// カメラ演出
		cameraRotateOffset_.x = Lerps::ExponentialInterpolate(cameraRotateOffset_, rotate, kRotateSpeedAttenuationRate.x, 0.15f).x;
	}
	else {
		// 徐々に速度を上げる
		moveVel_.y = Lerps::ExponentialInterpolate(moveVel_, move, kMoveSpeedAttenuationRate.y, 0.05f).y;
		// レティクルが画面端でなければ回転
		if (!isVertical_) {
			rotateVel_.x = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedAttenuationRate.x, 0.1f).x;		
		}
		// カメラ演出
		cameraRotateOffset_.x = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedAttenuationRate.x, 0.1f).x;
	}
	// 横移動
	// 移動をしていない場合
	if (move.x == 0.0f) {
		// 徐々に速度を落とす
		moveVel_.x = Lerps::ExponentialInterpolate(moveVel_, move, kMoveSpeedAttenuationRate.x, 0.1f).x;
		rotateVel_.y = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedAttenuationRate.y, 0.1f).y;
		rotateVel_.z = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedAttenuationRate.z, 0.1f).z;
		// カメラ演出
		cameraRotateOffset_.y = Lerps::ExponentialInterpolate(cameraRotateOffset_, rotate, kRotateSpeedAttenuationRate.y, 0.15f).y;
		cameraRotateOffset_.z = Lerps::ExponentialInterpolate(cameraRotateOffset_, rotate, kRotateSpeedAttenuationRate.z, 0.15f).z;
	}
	else {
		// 徐々に速度を上げる
		moveVel_.x = Lerps::ExponentialInterpolate(moveVel_, move, kMoveSpeedAttenuationRate.x, 0.05f).x;
		// レティクルが画面端でなければ回転
		if (!isHorizontal_) {
			rotateVel_.y = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedAttenuationRate.y, 0.1f).y;
			rotateVel_.z = Lerps::ExponentialInterpolate(rotateVel_, rotate, kRotateSpeedAttenuationRate.z, 0.1f).z;
		}
		// カメラ演出
		cameraRotateOffset_.y = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedAttenuationRate.y, 0.1f).y;
		cameraRotateOffset_.z = Lerps::ExponentialInterpolate(cameraRotateOffset_, kMaxCameraRotDirection * rotate, kRotateSpeedAttenuationRate.z, 0.1f).z;
	}
#pragma endregion
	// 加速中なら速度を上げる
	if (!isBoost_) {
		// 徐々に速度を落とす
		object3d_->worldTransform.translate.z = Lerps::ExponentialInterpolate(object3d_->worldTransform.translate, move, kMoveSpeedAttenuationRate.z, 0.01f).z;
		moveVel_.z = 0;
		input_->GamePadVibration(0, 0, 0);
	}
	else {
		// 徐々に速度を上げる
		moveVel_.z = Lerps::ExponentialInterpolate(moveVel_, move, kMoveSpeedAttenuationRate.z, 1.0f).z;
		input_->GamePadVibration(0, 65535, 65535);
	}

	// 移動速度の上限
	moveVel_.x = std::clamp<float>(moveVel_.x, -kMaxSpeed, kMaxSpeed);
	moveVel_.y = std::clamp<float>(moveVel_.y, -kMaxSpeed, kMaxSpeed);
	moveVel_.z = std::clamp<float>(moveVel_.z, -1.0f, 1.0f);
	// 回転速度の上限
	rotateVel_.x = std::clamp<float>(rotateVel_.x, -kMaxRotSpeed.x, kMaxRotSpeed.x);
	rotateVel_.y = std::clamp<float>(rotateVel_.y, -kMaxRotSpeed.y, kMaxRotSpeed.y);
	rotateVel_.z = std::clamp<float>(rotateVel_.z, -kMaxRotSpeed.z, kMaxRotSpeed.z);

	// 求めた回転を代入
	object3d_->worldTransform.rotate.x = rotateVel_.x;
	object3d_->worldTransform.rotate.y = rotateVel_.y;
	object3d_->worldTransform.rotate.z = 0;
	// 速度を自機に加算
	object3d_->worldTransform.translate += moveVel_;

	// カメラ移動
	cameraOffset_ += moveVel_ / 2;
	// カメラの移動幅上限
	cameraOffset_.x = std::clamp<float>(cameraOffset_.x, -3.5f, 3.5f);
	cameraOffset_.y = std::clamp<float>(cameraOffset_.y, -3.5f, 3.5f);
	cameraOffset_.z = 0.0f;
	//cameraOffset_.z = std::clamp<float>(cameraOffset_.z, -3.5f, 3.5f);

	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();
}

void Player::Aim() {
	// 3Dレティクル配置
	Deploy3DReticle();

	// ロックオン処理(必ず3Dレティクル配置と2Dレティクル配置の間に書く)
	aimAssist_->LockOn(sprite2DReticle_[0]->GetPos());

	// 2Dレティクル配置
	Deploy2DReticle();

	// ロックオンしたら赤くなる
	if (*isLockOn_) {
		sprite2DReticle_[0]->SetColor(Vector4{ 1,0,0,1 });
	}
	else {
		sprite2DReticle_[0]->SetColor(Vector4{ 1,1,1,1 });
	}
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
			newBullet->SetCollisionManager(collisionManager_);
			newBullet->Initialize(models_[2], worldPos, velocity);
			// 弾を登録
			gameScene_->AddPlayerBullet(newBullet);

			// 弾の発射間隔リセット
			bulletInterval_ = kBulletInterval;
		}
	}

	bulletInterval_--;
	// 0未満にならないようにする
	bulletInterval_ = std::clamp<int>(bulletInterval_, 0, kBulletInterval);
}

void Player::DecrementHP() {
	// 今無敵か
	if (!isInvinsible_) {
		if (PressOnCollision()) {
			hpSize_.x -= kMaxHPSize.x / 3;
			hpSprite_->SetSize(hpSize_);
			// 1秒無敵
			invinsibleFrame_ = kMaxInvinsibleFrame;
			isInvinsible_ = true;
		}
	}
}

void Player::Deploy3DReticle() {
	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset{ 0, 0, kDistanceObject };
	// 回転行列を合成
	Matrix4x4 rotateMatrix = MakeRotateMatrix(object3d_->worldTransform.rotate + object3d_->worldTransform.parent_->rotate);
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
	// スプライトのレティクルに座標設定
	sprite2DReticle_[0]->SetPos(Vector2(positionReticle.x, positionReticle.y));

	// 真ん中のレティクル
	// ワールド→スクリーン座標変換
	positionReticle = GetWorld3DReticlePosition(1);
	positionReticle = Transforms(positionReticle, matViewProjectionViewport);
	// スプライトのレティクルに座標設定
	sprite2DReticle_[1]->SetPos(Vector2(positionReticle.x, positionReticle.y));

#pragma region 画面端の処理
	isHorizontal_ = false;
	isVertical_ = false;

	// 左右
	if (sprite2DReticle_[0]->GetPos().x > WinApp::kClientWidth_ + 1.0f || sprite2DReticle_[0]->GetPos().x < 0.0f - 1.0f) {
		//// 左右どちらかの画面端にいる
		//isHorizontal_ = true;
		//// 左右どちらの画面端にいるかを検出し、レティクル座標を合わせる
		//// 右
		//if (sprite2DReticle_[0]->GetPos().x > (float)WinApp::kClientWidth_ + 1.0f) {
		//	sprite2DReticle_[0]->SetPosX((float)WinApp::kClientWidth_);
		//}
		//// 左
		//else if (sprite2DReticle_[0]->GetPos().x < 0.0f - 1.0f) {
		//	sprite2DReticle_[0]->SetPosX(0.0f);
		//}

		//// 合成行列の逆行列を計算する
		//Matrix4x4 matInverseVPV = Inverse(matViewProjectionViewport);
		//// スクリーン座標
		//Vector3 posNear = Vector3((float)sprite2DReticle_[0]->GetPos().x, (float)sprite2DReticle_[0]->GetPos().y, 0);
		//Vector3 posFar = Vector3((float)sprite2DReticle_[0]->GetPos().x, (float)sprite2DReticle_[0]->GetPos().y, 1);
		//// スクリーン座標系からワールド座標系へ
		//posNear = Transforms(posNear, matInverseVPV);
		//posFar = Transforms(posFar, matInverseVPV);
		//// マウスレイの方向
		//Vector3 mouseDirection = Subtract(posFar, posNear);
		//mouseDirection = Normalize(mouseDirection);
		//// 3Dレティクルを2Dカーソルに配置
		//object3dReticle_->worldTransform.translate = posNear - mouseDirection * kDistanceObject;
		//object3dReticle_->worldTransform.UpdateMatrix();
	}
	// 上下
	//if (sprite2DReticle_->GetPos().y > WinApp::kClientHeight_ + 1.0f || sprite2DReticle_->GetPos().y < 0.0f - 1.0f) {
	//	isVertical_ = true;
	//	// 上下どちらの画面端にいるかを検出し、レティクル座標を合わせる
	//	// 下
	//	if (sprite2DReticle_->GetPos().y > (float)WinApp::kClientHeight_ + 1.0f) {
	//		sprite2DReticle_->SetPosY((float)WinApp::kClientHeight_);
	//	}
	//	// 上
	//	else if (sprite2DReticle_->GetPos().y < 0.0f - 1.0f) {
	//		sprite2DReticle_->SetPosY(0.0f);
	//	}
	//}
#pragma endregion
}