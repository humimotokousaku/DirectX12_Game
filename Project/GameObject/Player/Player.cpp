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

	// キーボード入力
	if (input_->PressKey(DIK_UP)) {
		object3d_->worldTransform.translate.y += 0.1f;
	}
	else if (input_->PressKey(DIK_DOWN)) {
		object3d_->worldTransform.translate.y -= 0.1f;
	}
	else if (input_->PressKey(DIK_RIGHT)) {
		object3d_->worldTransform.translate.x += 0.1f;
	}
	else if (input_->PressKey(DIK_LEFT)) {
		object3d_->worldTransform.translate.x -= 0.1f;
	}
	// 入力がない時
	if (input_->DetectKeyInput()) {
		move = { 0,0,0 };
	}
	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState_)) {
		// デッドゾーンの設定
		SHORT leftThumbX = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbLX);
		SHORT leftThumbY = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbLY);
		move.x += (float)leftThumbX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)leftThumbY / SHRT_MAX * kCharacterSpeed;
	}
	// 入力がない時
	if (input_->DetectThumbInput(joyState_.Gamepad.sThumbLX, joyState_.Gamepad.sThumbLY)) {
		move = { 0,0,0 };
	}

	/// 移動処理↑

	// 旋回処理
	Rotate();

	object3d_->worldTransform.UpdateMatrix();

	// 3Dレティクルの配置
	Deploy3DReticle();
	// 2Dレティクルを配置
	Deploy2DReticle();

	// 弾の発射処理
	Attack();

	// 移動限界座標
	const Vector2 kMoveLimit = { 7.0f, 4.0f };
	// 範囲を超えない処理
	object3d_->worldTransform.translate.x = max(object3d_->worldTransform.translate.x, -kMoveLimit.x);
	object3d_->worldTransform.translate.x = min(object3d_->worldTransform.translate.x, kMoveLimit.x);
	object3d_->worldTransform.translate.y = max(object3d_->worldTransform.translate.y, -kMoveLimit.y);
	object3d_->worldTransform.translate.y = min(object3d_->worldTransform.translate.y, kMoveLimit.y);
	// 座標移動
	object3d_->worldTransform.translate.x += move.x;
	object3d_->worldTransform.translate.y += move.y;
	object3d_->worldTransform.translate.z += move.z;

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

void Player::Rotate() {
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;
	Vector2 joyRange{};
	// 押した方向で移動ベクトルを変更
	if (input_->PressKey(DIK_A)) {
		object3d_->worldTransform.rotate.y -= kRotSpeed;
	}
	else if (input_->PressKey(DIK_D)) {
		object3d_->worldTransform.rotate.y += kRotSpeed;
	}
	if (input_->PressKey(DIK_W)) {
		object3d_->worldTransform.rotate.x -= kRotSpeed;
	}
	else if (input_->PressKey(DIK_S)) {
		object3d_->worldTransform.rotate.x += kRotSpeed;
	}

	// ゲームパッド
	if (Input::GetInstance()->GetJoystickState(0, joyState_)) {
		// デッドゾーンの設定
		SHORT leftThumbX = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbRX);
		SHORT leftThumbY = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbRY);
		joyRange.y += (float)leftThumbY / SHRT_MAX / 62.8f;
		joyRange.x += (float)leftThumbX / SHRT_MAX / 62.8f;
		object3d_->worldTransform.rotate.x -= joyRange.y;
		object3d_->worldTransform.rotate.y += joyRange.x;
		object3d_->worldTransform.UpdateMatrix();
	}
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