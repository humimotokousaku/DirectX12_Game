#include "Player.h"
#include "TextureManager.h"
#include "CollisionConfig.h"
#include "GameTime.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "Input.h"
#include <numbers>

Player::Player() {}
Player::~Player() {
	models_.clear();
	delete sprite2DReticle_;
	//collisionManager_->ClearColliderList(this);
}

void Player::Initialize() {
	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	// 移動キーの登録
	moveKeys_.resize(4);
	moveKeys_[0] = DIK_UP;
	moveKeys_[1] = DIK_DOWN;
	moveKeys_[2] = DIK_LEFT;
	moveKeys_[3] = DIK_RIGHT;
	// 回転キーの登録
	rotateKeys_.resize(4);
	rotateKeys_[0] = DIK_W;
	rotateKeys_[1] = DIK_A;
	rotateKeys_[2] = DIK_S;
	rotateKeys_[3] = DIK_D;

	// 自機モデル作成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetModel(models_[0]);
	object3d_->SetCamera(camera_);
	object3d_->worldTransform.translate = { 0,-2,10 };

	// 3Dレティクルモデル作成
	object3dReticle_ = std::make_unique<Object3D>();
	object3dReticle_->Initialize();
	object3dReticle_->SetModel(models_[1]);
	object3dReticle_->SetCamera(camera_);
	gameSpeed_ = 1.0f;
	// 2Dレティクル作成
	sprite2DReticle_ = Sprite::Create("", "reticle.png");
	sprite2DReticle_->SetPos(Vector2(WinApp::kClientWidth_ / 2, WinApp::kClientHeight_ / 2));

	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
	collisionManager_->SetColliderList(this);
}

void Player::Update() {
	// 移動処理
	Move();
	// 旋回処理
	//Rotate();
	Aim();
	// 弾の発射処理
	Attack();

	// ワールド行列を更新
	object3d_->worldTransform.UpdateMatrix();
	// 移動限界座標
	const Vector2 kMoveLimit = { 7.0f, 4.0f };
	// 範囲を超えない処理
	object3d_->worldTransform.translate.x = max(object3d_->worldTransform.translate.x, -kMoveLimit.x);
	object3d_->worldTransform.translate.x = min(object3d_->worldTransform.translate.x, kMoveLimit.x);
	object3d_->worldTransform.translate.y = max(object3d_->worldTransform.translate.y, -kMoveLimit.y);
	object3d_->worldTransform.translate.y = min(object3d_->worldTransform.translate.y, kMoveLimit.y);

	// 自機の回転上限
	object3d_->worldTransform.rotate.x = std::clamp<float>(object3d_->worldTransform.rotate.x, -0.7f, 0.7f);
	object3d_->worldTransform.rotate.y = std::clamp<float>(object3d_->worldTransform.rotate.y, -0.7f, 0.7f);
	object3d_->worldTransform.rotate.z = std::clamp<float>(object3d_->worldTransform.rotate.z, -0.7f, 0.7f);

	// ImGui
	object3d_->ImGuiParameter("Player");
	ImGui::DragFloat3("moveVel", &moveVel_.x, 0);
	ImGui::DragFloat3("rotateVel", &rotateVel_.x, 0);
}

void Player::Draw() {
	// 自機
	object3d_->Draw();
	// 3Dレティクル
	object3dReticle_->Draw();
}

void Player::DrawUI() {
	sprite2DReticle_->Draw();
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	object3d_->worldTransform.parent_ = parent;
}

void Player::OnCollision(Collider* collider) {

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

Vector3 Player::GetWorld3DReticlePosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos = object3dReticle_->worldTransform.translate;
	// ワールド行列の平行移動成分を取得
	worldPos.x = object3dReticle_->worldTransform.matWorld_.m[3][0];
	worldPos.y = object3dReticle_->worldTransform.matWorld_.m[3][1];
	worldPos.z = object3dReticle_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}

void Player::Move() {
	// キャラクターの移動ベクトル
	Vector3 move{};
	// 自機の最大移動速度
	const float kMaxSpeed = 0.1f;
	// 自機の回転
	Vector3 rotate{};
	const float kMaxRotSpeed = 0.05f;

#pragma region キーボード
	// キーボード入力
	if (input_->PressKey(DIK_UP)) {
		move.y += kMaxSpeed;
		rotate.x -= kMaxRotSpeed;
	}
	if (input_->PressKey(DIK_DOWN)) {
		move.y -= kMaxSpeed;
		rotate.x += kMaxRotSpeed;
	}
	if (input_->PressKey(DIK_RIGHT)) {
		move.x += kMaxSpeed;
		rotate.y += kMaxRotSpeed;
		rotate.z -= kMaxRotSpeed;
	}
	if (input_->PressKey(DIK_LEFT)) {
		move.x -= kMaxSpeed;
		rotate.y -= kMaxRotSpeed;
		rotate.z += kMaxRotSpeed;
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
		rotate.y += (float)leftThumbX * kMaxRotSpeed;
		rotate.z -= (float)leftThumbX * kMaxRotSpeed;
		rotate.x -= (float)leftThumbY * kMaxRotSpeed;
	}
#pragma endregion

#pragma region 指数補間で速度を徐々に上げる,下げる
	// 縦移動
	// 移動をしていない場合
	if (move.y == 0.0f) {
		move.y = 0.0f;
		rotate.x = 0.0f;
		// 徐々に速度を落とす
		moveVel_.y = Lerps::ExponentialInterpolate(moveVel_, move, 4.0f, 0.1f).y;
		rotateVel_.x = Lerps::ExponentialInterpolate(rotateVel_, rotate, 4.0f, 0.1f).x;
		// 機体の向きを徐々に戻す
		object3d_->worldTransform.rotate.x = Lerps::ExponentialInterpolate(object3d_->worldTransform.rotate, rotate, 4.0f, 0.05f).x;
	}
	else {
		// 徐々に速度を上げる
		moveVel_.y = Lerps::ExponentialInterpolate(moveVel_, move, 4.0f, 0.1f).y;
		rotateVel_.x = Lerps::ExponentialInterpolate(rotateVel_, rotate, 2.0f, 0.1f).x;
	}
	// 横移動
	// 移動をしていない場合
	if (move.x == 0.0f) {
		move.x = 0.0f;
		rotate.y = 0.0f;
		rotate.z = 0.0f;
		// 徐々に速度を落とす
		moveVel_.x = Lerps::ExponentialInterpolate(moveVel_, move, 4.0f, 0.1f).x;
		rotateVel_.y = Lerps::ExponentialInterpolate(rotateVel_, rotate, 4.0f, 0.1f).y;
		rotateVel_.z = Lerps::ExponentialInterpolate(rotateVel_, rotate, 4.0f, 0.1f).z;
		// 機体の向きを徐々に戻す
		object3d_->worldTransform.rotate.y = Lerps::ExponentialInterpolate(object3d_->worldTransform.rotate, rotate, 4.0f, 0.05f).y;
		object3d_->worldTransform.rotate.z = Lerps::ExponentialInterpolate(object3d_->worldTransform.rotate, rotate, 4.0f, 0.05f).z;
	}
	else {
		// 徐々に速度を上げる
		moveVel_.x = Lerps::ExponentialInterpolate(moveVel_, move, 4.0f, 0.1f).x;
		rotateVel_.y = Lerps::ExponentialInterpolate(rotateVel_, rotate, 2.0f, 0.1f).y;
		rotateVel_.z = Lerps::ExponentialInterpolate(rotateVel_, rotate, 2.0f, 0.1f).z;
	}
#pragma endregion

	// 移動速度の上限
	moveVel_.x = std::clamp<float>(moveVel_.x, -kMaxSpeed, kMaxSpeed);
	moveVel_.y = std::clamp<float>(moveVel_.y, -kMaxSpeed, kMaxSpeed);
	moveVel_.z = std::clamp<float>(moveVel_.z, -kMaxSpeed, kMaxSpeed);
	// 回転速度の上限
	rotateVel_.x = std::clamp<float>(rotateVel_.x, -kMaxRotSpeed, kMaxRotSpeed);
	rotateVel_.y = std::clamp<float>(rotateVel_.y, -kMaxRotSpeed, kMaxRotSpeed);
	rotateVel_.z = std::clamp<float>(rotateVel_.z, -kMaxRotSpeed, kMaxRotSpeed);

	// 速度を足す
	object3d_->worldTransform.translate += moveVel_;
	object3d_->worldTransform.rotate += rotateVel_;
}

//void Player::Rotate() {
//	Vector3 rotate = { 0,0,0 };
//	// 回転速さ[ラジアン/frame]
//	const float kRotSpeed = 0.02f;
//	Vector2 joyRange{};
//	// 押した方向で移動ベクトルを変更
//	if (input_->PressKey(DIK_A)) {
//		rotate.y -= kRotSpeed;
//	}
//	if (input_->PressKey(DIK_D)) {
//		rotate.y += kRotSpeed;
//	}
//	if (input_->PressKey(DIK_W)) {
//		rotate.x -= kRotSpeed;
//	}
//	if (input_->PressKey(DIK_S)) {
//		rotate.x += kRotSpeed;
//	}
//	// 入力がない時
//	if (input_->DetectKeyInput(rotateKeys_)) {
//		rotate = { 0,0,0 };
//		// 徐々に速度を落とす
//		rotateVel_ = Lerps::ExponentialInterpolate(rotateVel_, rotate, 2.5f, 0.1f);
//	}
//	else {
//		rotateVel_ = Lerps::ExponentialInterpolate(rotateVel_, rotate, 2.5f, 0.1f);
//	}
//
//	// ゲームパッド
//	if (Input::GetInstance()->GetJoystickState(0, joyState_)) {
//		// デッドゾーンの設定
//		SHORT leftThumbX = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbRX);
//		SHORT leftThumbY = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbRY);
//		joyRange.y += (float)leftThumbY / SHRT_MAX / 62.8f;
//		joyRange.x += (float)leftThumbX / SHRT_MAX / 62.8f;
//		object3d_->worldTransform.rotate.x -= joyRange.y;
//		object3d_->worldTransform.rotate.y += joyRange.x;
//		object3d_->worldTransform.UpdateMatrix();
//	}
//
//	// 回転速度の制限
//	rotateVel_.x = std::clamp<float>(rotateVel_.x, -0.04f, 0.04f);
//	rotateVel_.y = std::clamp<float>(rotateVel_.y, -0.04f, 0.04f);
//
//	// 回転を足す
//	object3d_->worldTransform.rotate += rotateVel_;
//
//	// 回転角の制限
//	object3d_->worldTransform.rotate.x = std::clamp<float>(object3d_->worldTransform.rotate.x, -0.4f, 0.4f);
//	object3d_->worldTransform.rotate.y = std::clamp<float>(object3d_->worldTransform.rotate.y, -0.74f, 0.74f);
//}

void Player::Aim() {
	Vector3 move = { 0,0,0 };
	// レティクルの最大移動速度
	const float kMaxSpeed = 16.0f;
	// 押した方向で移動ベクトルを変更
	if (input_->PressKey(DIK_A)) {
		move.x -= kMaxSpeed;
	}
	if (input_->PressKey(DIK_D)) {
		move.x += kMaxSpeed;
	}
	if (input_->PressKey(DIK_W)) {
		move.y -= kMaxSpeed;
	}
	if (input_->PressKey(DIK_S)) {
		move.y += kMaxSpeed;
	}
	// 入力がない時
	if (input_->DetectKeyInput(rotateKeys_)) {
		move = { 0,0,0 };
		// 徐々に速度を落とす
		reticleMoveVel_ = Lerps::ExponentialInterpolate(reticleMoveVel_, move, 4.0f, 0.1f);
	}
	else {
		reticleMoveVel_ = Lerps::ExponentialInterpolate(reticleMoveVel_, move, 4.0f, 0.1f);
	}
	// レティクルの移動速度を制限
	reticleMoveVel_.x = std::clamp<float>(reticleMoveVel_.x, -kMaxSpeed, kMaxSpeed);
	reticleMoveVel_.y = std::clamp<float>(reticleMoveVel_.y, -kMaxSpeed, kMaxSpeed);

#pragma region 2Dレティクルの配置
	Vector2 spritePos = sprite2DReticle_->GetPos() + Vector2{ reticleMoveVel_.x, reticleMoveVel_.y };
	// ジョイスティック状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState_)) {
		Vector2 joyRange{};
		// デッドゾーンの設定
		SHORT rightThumbX = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbRX);
		SHORT rightThumbY = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbRY);
		joyRange.x += (float)rightThumbX / SHRT_MAX * 10.0f;
		joyRange.y += (float)rightThumbY / SHRT_MAX * 10.0f;
		spritePos.x += joyRange.x;
		spritePos.y -= joyRange.y;
	}
	// レティクルの移動制限
	spritePos.x = std::clamp<float>(spritePos.x, 0, WinApp::kClientWidth_);
	spritePos.y = std::clamp<float>(spritePos.y, 0, WinApp::kClientHeight_);
	// スプライトへの座標変更を反映
	sprite2DReticle_->SetPos(spritePos);
#pragma endregion

#pragma region 2Dレティクルから3Dレティクルに変換
	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0, 1);
	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matVPV = Multiply(camera_->GetViewProjection().matView, Multiply(camera_->GetViewProjection().matProjection, matViewport));
	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// スクリーン座標
	Vector3 posNear = Vector3(sprite2DReticle_->GetPos().x, sprite2DReticle_->GetPos().y, 0);
	Vector3 posFar = Vector3(sprite2DReticle_->GetPos().x, sprite2DReticle_->GetPos().y, 1);

	// スクリーン座標系からワールド座標系へ
	posNear = Transforms(posNear, matInverseVPV);
	posFar = Transforms(posFar, matInverseVPV);
	// マウスレイの方向
	Vector3 mouseDirection = Subtract(posFar, posNear);
	mouseDirection = Normalize(mouseDirection);
	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 100.0f;
	// 3Dレティクルを2Dカーソルに配置
	object3dReticle_->worldTransform.translate.x = posNear.x - mouseDirection.x * kDistanceTestObject;
	object3dReticle_->worldTransform.translate.y = posNear.y - mouseDirection.y * kDistanceTestObject;
	object3dReticle_->worldTransform.translate.z = posNear.z - mouseDirection.z * kDistanceTestObject;

	object3dReticle_->worldTransform.UpdateMatrix();
#pragma endregion
}

void Player::Attack() {
	// Rトリガーを押していたら
	if (joyState_.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER || Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		// 速度ベクトルを自機の向きに合わせて回転させる
		Vector3 worldPos = GetWorldPosition();
		velocity = TransformNormal(velocity, object3d_->worldTransform.matWorld_);
		// 自機から照準オブジェクトへのベクトル
		Vector3 worldReticlePos = {
			object3dReticle_->worldTransform.matWorld_.m[3][0], object3dReticle_->worldTransform.matWorld_.m[3][1],
			object3dReticle_->worldTransform.matWorld_.m[3][2] };
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
	}
}

void Player::Deploy3DReticle() {
	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;
	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset{ 0, 0, 1.0f };
	// 自機のワールド行列の回転を反映する
	offset = TransformNormal(offset, object3d_->worldTransform.matWorld_);
	offset = Normalize(offset);
	// ベクトルの長さを整える
	offset *= kDistancePlayerTo3DReticle;

	// 3Dレティクルの座標を設定
	object3dReticle_->worldTransform.translate = GetWorldPosition() + offset;

	// 3Dレティクルのワールド行列を更新
	object3dReticle_->worldTransform.UpdateMatrix();
}

void Player::Deploy2DReticle() {
	// 3Dレティクルのワールド座標を取得
	Vector3 positionReticle = GetWorld3DReticlePosition();

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport{};
	matViewProjectionViewport =
		Multiply(camera_->GetViewProjection().matView, Multiply(camera_->GetViewProjection().matProjection, matViewport));
	// ワールド→スクリーン座標変換
	positionReticle = Transforms(positionReticle, matViewProjectionViewport);
	// スプライトのレティクルに座標設定
	sprite2DReticle_->SetPos(Vector2(positionReticle.x, positionReticle.y));
}