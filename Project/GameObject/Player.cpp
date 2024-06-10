#include "Player.h"
#include "TextureManager.h"
#include "Collision/CollisionConfig.h"
#include "GameTime.h"
#include "ImGuiManager.h"
#include <numbers>

Player::Player() {}
Player::~Player() {
	for (Object3D* model : models_) {
		delete model;
	}
	models_.clear();
}

void Player::Initialize(Camera* camera) {
	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);

	// モデル作成
	object3d_ = std::make_unique<Object3D>();
	object3d_->Initialize();
	// モデルを設定
	object3d_->SetModel("Human", "walk.gltf");
	object3d_->SetAnimName("Walk");
	object3d_->SetCamera(camera);
	// しゃがみ歩きアニメーション追加
	object3d_->AddAnimation("Human", "sneakWalk.gltf");
	object3d_->StartAnim("Walk");

	object3d_->worldTransform.transform.translate = { 0,-2,5 };
	object3d_->worldTransform.transform.rotate = { 0,std::numbers::inv_pi * 5.0f,0 };
	gameSpeed_ = 1.0f;
}

void Player::Update() {

	GameTimer::GetInstance()->SetDeltaTimeMultiply(gameSpeed_);
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// デッドゾーンの設定
		SHORT leftThumbX = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbLX);
		SHORT leftThumbY = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbLY);
		object3d_->worldTransform.transform.translate.x += (float)leftThumbX / SHRT_MAX * 0.02f * GameTimer::GetInstance()->GetDeltaTime();
		object3d_->worldTransform.transform.translate.z += (float)leftThumbY / SHRT_MAX * 0.02f * GameTimer::GetInstance()->GetDeltaTime();
		if (leftThumbX > 0.0f) {
			object3d_->worldTransform.transform.rotate.y = std::numbers::inv_pi * 5.0f;
		}
		else if(leftThumbX < 0.0f) {
			object3d_->worldTransform.transform.rotate.y = -std::numbers::inv_pi * 5.0f;
		}
	}

	if (Input::GetInstance()->GamePadPress(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		object3d_->StartAnim("SneakWalk");
	}
	else {
		object3d_->StartAnim("Walk");
	}

	// ImGui
	object3d_->ImGuiParameter("Player");

	ImGui::Begin("gameSpeed");
	ImGui::DragFloat("gameSpeed", &gameSpeed_, 0.01f, 0, 10);
	ImGui::End();
}

void Player::Draw(uint32_t textureHandle) {
	// player
	object3d_->Draw(textureHandle);
}

void Player::OnCollision(Collider* collider) {

}

Vector3 Player::GetRotation() {
	Vector3 rotate = object3d_->worldTransform.transform.rotate;
	return rotate;
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos = object3d_->worldTransform.transform.translate;
	//// ワールド行列の平行移動成分を取得
	//worldPos.x = model_->worldTransform.matWorld_.m[3][0];
	//worldPos.y = model_->worldTransform.matWorld_.m[3][1];
	//worldPos.z = model_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}