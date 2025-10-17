#include "Player.h"
#include <algorithm>

Player::Player(Camera* camera) {
	// 初期位置の設定
	mPosition = { 0.0f, 0.0f };
	mTag = "PlayerTag";
	mName = "Player";
	mAttackPower = 10;

	// オブジェクト
	obj_ = std::make_unique<Object3D>();
	obj_->Initialize();
	obj_->SetModel("Models", "cube.obj");
	obj_->SetCamera(camera);

	//Vector2 min = mPosition - Vector2{ 30.f, 30.f };
	//Vector2 max = mPosition + Vector2{ 30.f, 30.f };
	//mCollider = MyRectangle{ min, max };

	//mCoolTimer.reset(cCoolTime);
}

Player::~Player() {

}

void Player::update(float delta_time) {
	Vector2 lMove{};
	Vector2 rMove{};

	// ゲームパッド状態取得
	if (input_->GetJoystickState(0, joyState_)) {
		// デッドゾーンの設定
		SHORT lThumbX = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbLX);
		SHORT lThumbY = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbLY);
		SHORT rThumbX = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbRX);
		SHORT rThumbY = Input::GetInstance()->ApplyDeadzone(joyState_.Gamepad.sThumbRY);

		lMove.x = (float)(lThumbX / SHRT_MAX);
		lMove.y = (float)(lThumbY / SHRT_MAX);
		rMove.x = (float)(rThumbX / SHRT_MAX);
		rMove.y = (float)(rThumbY / SHRT_MAX);
	}
	// 
	DifferentialUpdate(lMove.y, rMove.y, delta_time);


	
	//if (!mIsEnableCollider) {
	//	mCoolTimer.update(delta_time);

	//	if (mCoolTimer.is_end()) {
	//		mIsEnableCollider = true;
	//	}
	//}

	//mVelocity = Vector2::zero();
	//Vector2 velocity = Vector2::zero();

	//// CheckHitKeyを数値化して移動方向を計算
	//// trueは1、falseは0として返される
	//velocity.y = static_cast<float>(CheckHitKey(KEY_INPUT_S) - CheckHitKey(KEY_INPUT_W));
	//velocity.x = static_cast<float>(CheckHitKey(KEY_INPUT_D) - CheckHitKey(KEY_INPUT_A));

	//// 入力がされているときのみ計算
	//if (velocity.magnitude() > 0.f) {
	//	mVelocity = velocity.normalized() * mSpeed;

	//	mPosition += mVelocity * delta_time;
	//}

	//mCollider = mCollider.translate(mVelocity * delta_time);

	//mHealthBar.update(delta_time, mHealth.GetHealth());
}

void Player::draw() const {
	obj_->Draw();
	//DrawCircle(mPosition.int_x(), mPosition.int_y(), 32, GetColor(0, 255, 0), TRUE);
}

void Player::draw_gui() {
	if (ImGui::TreeNode("Player")) {
		ImGui::DragFloat("MaxSpeed", &maxSpeed, 0.01f, -1000.0f, 1000.0f);
		ImGui::DragFloat("TreadWidth", &treadWidth, 0.01f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Omega", &omega);
		ImGui::DragFloat("Angle", &angle);
		ImGui::TreePop();
	}
	//mCollider.draw_debug();
}

void Player::InputHandle() {

}

void Player::DifferentialUpdate(float leftStickY, float rightStickY, float deltaTime) {
	// 履帯の目標速度（入力を反転したい場合は符号調整）
	float target_vL = leftStickY * maxSpeed;
	float target_vR = rightStickY * maxSpeed;

	// 補間（スムーズな操作）
	vL += (target_vL - vL) * 0.1f;
	vR += (target_vR - vR) * 0.1f;

	// 差動モデル計算
	float v = (vR + vL) * 0.5f;
	omega = (vR - vL) / treadWidth;

	// 更新
	angle += omega * deltaTime;

	// 速度
	Vector3 vel = {
		v * cosf(angle) * deltaTime,
		0.0f,
		v * sinf(angle) * deltaTime
	};
	obj_->worldTransform.translate += vel;

	// yaw（ヨー角）: 水平回転
	obj_->worldTransform.rotate.y = atan2f(vel.x, vel.z);
	// pitch（ピッチ角）: 上下の傾き
	obj_->worldTransform.rotate.x = atan2f(-vel.y, sqrtf(vel.x * vel.x + vel.z * vel.z));
}
