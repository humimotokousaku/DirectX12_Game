#include "Player.h"
#include "TextureManager.h"
#include "Collision/CollisionConfig.h"
#include "GameTime.h"
#include "ImGuiManager.h"
#include <numbers>

Player::Player() {}
Player::~Player() {
	//delete model_;
}

void Player::Initialize(Camera* camera) {
	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);

	// モデル作成
	model_ = std::make_unique<Object3D>();
	model_->Initialize();
	// モデルを設定
	model_->SetModel("Human", "walk.gltf");
	model_->SetAnimName("Walk");
	model_->SetCamera(camera);
	// しゃがみ歩きアニメーション追加
	model_->AddAnimation("Human", "sneakWalk.gltf");

	model_->StartAnim("Walk");
	model_->worldTransform.transform.translate = { 0,-2,-15 };
	model_->worldTransform.transform.rotate = { 0,std::numbers::inv_pi * 5.0f,0 };

	//model_->animation_.isActive = true;
	gameSpeed_ = 1.0f;
}

void Player::Update() {

	GameTimer::GetInstance()->SetDeltaTimeMultiply(gameSpeed_);
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// デッドゾーンの設定
		SHORT leftThumbX = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbLX);
		SHORT leftThumbY = Input::GetInstance()->ApplyDeadzone(joyState.Gamepad.sThumbLY);
		model_->worldTransform.transform.translate.x += (float)leftThumbX / SHRT_MAX * 0.02f * GameTimer::GetInstance()->GetDeltaTime();
		model_->worldTransform.transform.translate.z += (float)leftThumbY / SHRT_MAX * 0.02f * GameTimer::GetInstance()->GetDeltaTime();
		if (leftThumbX > 0.0f) {
			model_->worldTransform.transform.rotate.y = std::numbers::inv_pi * 5.0f;
		}
		else if(leftThumbX < 0.0f) {
			model_->worldTransform.transform.rotate.y = -std::numbers::inv_pi * 5.0f;
		}
	}

	if (Input::GetInstance()->GamePadPress(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		model_->StartAnim("SneakWalk");
	}
	else {
		model_->StartAnim("Walk");
	}

	// ImGui
	model_->ImGuiParameter("Player");

	ImGui::Begin("gameSpeed");
	ImGui::DragFloat("gameSpeed", &gameSpeed_, 0.01f, 0, 10);
	ImGui::End();
}

void Player::Draw(uint32_t textureHandle) {
	// player
	model_->Draw(textureHandle);
}

void Player::OnCollision(Collider* collider) {

}

Vector3 Player::GetRotation() {
	Vector3 rotate = model_->worldTransform.transform.rotate;
	return rotate;
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos = model_->worldTransform.transform.translate;
	//// ワールド行列の平行移動成分を取得
	//worldPos.x = model_->worldTransform.matWorld_.m[3][0];
	//worldPos.y = model_->worldTransform.matWorld_.m[3][1];
	//worldPos.z = model_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}