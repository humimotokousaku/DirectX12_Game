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
	//collisionManager_->ClearColliderList(this);
}

void Player::Initialize() {
	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// 自機モデル作成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	object3d_->SetModel(models_[0]);
	object3d_->SetCamera(camera_);
	object3d_->worldTransform.translate = { 0,-2,50 };

	// 3Dレティクルモデル作成
	object3dReticle_ = std::make_unique<Object3D>();
	object3dReticle_->Initialize();
	object3dReticle_->SetModel(models_[1]);
	object3dReticle_->SetCamera(camera_);
	gameSpeed_ = 1.0f;

	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);
	collisionManager_->SetColliderList(this);
}

void Player::Update() {
	// キャラクターの移動ベクトル
	Vector3 move = { 0, 0, 0 };
	// キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

	/// 移動処理↓

	XINPUT_STATE joyState;
	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// デッドゾーンの設定
		SHORT leftThumbX = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbLX);
		SHORT leftThumbY = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbLY);
		move.x += (float)leftThumbX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)leftThumbY / SHRT_MAX * kCharacterSpeed;
	}
	// 移動限界座標
	const Vector2 kMoveLimit = { 40 - 10, 30 - 15 };
	// 範囲を超えない処理
	object3d_->worldTransform.translate.x = max(object3d_->worldTransform.translate.x, -kMoveLimit.x);
	object3d_->worldTransform.translate.x = min(object3d_->worldTransform.translate.x, kMoveLimit.x);
	object3d_->worldTransform.translate.y = max(object3d_->worldTransform.translate.y, -kMoveLimit.y);
	object3d_->worldTransform.translate.y = min(object3d_->worldTransform.translate.y, kMoveLimit.y);
	// 座標移動
	object3d_->worldTransform.translate.x += move.x;
	object3d_->worldTransform.translate.y += move.y;
	object3d_->worldTransform.translate.z += move.z;

	/// 移動処理↑

	// 旋回処理
	Rotate();

	object3d_->worldTransform.UpdateMatrix();

	// 弾の処理
	Attack();

	// 3Dレティクルの配置
	Deploy3DReticle();
	// 2Dレティクルを配置
	//Deploy2DReticle(viewProjection);

	// 3Dレティクルの移動
	Vector2 joyRange{};
	// ジョイスティック状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// デッドゾーンの設定
		SHORT rightThumbX = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbRX);
		SHORT rightThumbY = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbRY);
		joyRange.y += (float)rightThumbY / SHRT_MAX / 62.8f;
		joyRange.x += (float)rightThumbX / SHRT_MAX / 62.8f;
		object3d_->worldTransform.rotate.x -= joyRange.y;
		object3d_->worldTransform.rotate.y += joyRange.x;
		object3d_->worldTransform.UpdateMatrix();
	}

#pragma endregion


	// ImGui
	object3d_->ImGuiParameter("Player");
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

// playerの回転
void Player::Rotate() {
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PressKey(DIK_A)) {
		object3d_->worldTransform.rotate.y -= kRotSpeed;
	}
	else if (input_->PressKey(DIK_D)) {
		object3d_->worldTransform.rotate.y += kRotSpeed;
	}
}

// 攻撃
void Player::Attack() {
	XINPUT_STATE joyState;
	// ゲームパッド未接続なら何もせず抜ける
	//if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
	//	return;
	//}

	// Rトリガーを押していたら
	if (/*joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ||*/ Input::GetInstance()->TriggerKey(DIK_SPACE)) {
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
		velocity = Subtract(worldReticlePos, worldPos);
		velocity = Normalize(velocity);
		velocity.x *= kBulletSpeed;
		velocity.y *= kBulletSpeed;
		velocity.z *= kBulletSpeed;

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
	offset.x *= kDistancePlayerTo3DReticle;
	offset.y *= kDistancePlayerTo3DReticle;
	offset.z *= kDistancePlayerTo3DReticle;

	// 3Dレティクルの座標を設定
	object3dReticle_->worldTransform.translate.x = GetWorldPosition().x + offset.x;
	object3dReticle_->worldTransform.translate.y = GetWorldPosition().y + offset.y;
	object3dReticle_->worldTransform.translate.z = GetWorldPosition().z + offset.z;

	object3dReticle_->worldTransform.UpdateMatrix();
}

void Player::Deploy2DReticle(const ViewProjection& viewProjection) {
	//// 3Dレティクルのワールド座標を取得
	//Vector3 positionReticle = GetWorld3DReticlePosition();

	//// ビューポート行列
	//Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0, 1);
	//// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	//Matrix4x4 matViewProjectionViewport{};
	//matViewProjectionViewport =
	//	Multiply(viewProjection.matView, Multiply(viewProjection.matProjection, matViewport));
	//// ワールド→スクリーン座標変換
	//positionReticle = Transforms(positionReticle, matViewProjectionViewport);
	// スプライトのレティクルに座標設定
	//sprite2DReticle_->SetPosition(Vector2(positionReticle.x - 16, positionReticle.y - 16));
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