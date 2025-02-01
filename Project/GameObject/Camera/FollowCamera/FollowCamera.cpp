#include "FollowCamera.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Utility.h"
#include <assert.h>

FollowCamera::FollowCamera() {
	// カメラの生成
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
}

void FollowCamera::Initialize(Player* player) {
	player_ = player;

	// 被弾時の画面揺れ
	hitShake_ = new Shake();
	hitShake_->SetRange(Vector3{ 2.0f,2.0f,0.0f });
	hitShake_->SetTarget(&shakeOffset_);
	hitShake_->SetEndFrame(30.0f);

	// 射撃時の揺れ
	shotShake_ = new Shake();
	shotShake_->SetRange(Vector3{ 0.3f,0.3f,0.3f });
	shotShake_->SetTarget(&shotShakeOffset_);
	shotShake_->SetEndFrame(10.0f);

	// 被弾時のカメラの揺れる範囲
	const float endFrame = 5;
	const Vector3 end = { 0.3f, 0.3f,0.0f };
	shakeRangeAnim_.SetAnimData(hitShake_->GetRange(), *hitShake_->GetRange(), end, endFrame, Easings::EaseInExpo);
	shakeRangeAnim_.SetAnimData(hitShake_->GetRange(), end, Vector3{ 0,0,0 }, hitShake_->GetEndFrame() - endFrame, Easings::EaseInCubic);

	targetOffset_ = { 0, 5, -20 };
}

void FollowCamera::Update() {
	assert(player_);

	// 被弾演出
	HitUpdate();
	// 射撃演出
	ShotUpdate();

	// 親のオイラー角を取得
	camera_->worldTransform_.rotate = camera_->worldTransform_.parent_->rotate;

	// 追従対象からカメラまでのオフセット
	Vector3 offset = TargetOffset();

	// 座標をコピーしてオフセット分ずらす
	if (!isLockPos_) {
		camera_->worldTransform_.translate = camera_->worldTransform_.parent_->translate + offset + shakeOffset_ + shotShakeOffset_;
	}
	// 演出用のoffsetを加算
	camera_->worldTransform_.rotate += player_->GetCameraRotateOffset();

	camera_->worldTransform_.rotate.x = std::fmod(camera_->worldTransform_.rotate.x, (float)std::numbers::pi * 2.0f);
	camera_->worldTransform_.rotate.y = std::fmod(camera_->worldTransform_.rotate.y, (float)std::numbers::pi * 2.0f);
	camera_->worldTransform_.rotate.z = std::fmod(camera_->worldTransform_.rotate.z, (float)std::numbers::pi * 2.0f);

	// fov
	if (fov_) {
		camera_->viewProjection_.fovAngleY = *fov_;
	}
	// カメラの更新
	camera_->Update();
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->SetViewMatrix(Inverse(camera_->worldTransform_.matWorld_));

#ifdef _DEBUG
	// ImGui
	ImGui::Begin("FollowCamera");
	ImGui::DragFloat3("translation", &camera_->worldTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->worldTransform_.rotate.x, 0.1f);	
	ImGui::DragFloat3("offset", &targetOffset_.x, 0.1f, 0, 200);
	ImGui::DragFloat("fov", &camera_->viewProjection_.fovAngleY, 0.1f, -200, 200);
	hitShake_->ImGuiParameter();
	ImGui::End();
#endif // _DEBUG
}

void FollowCamera::HitUpdate() {
	// 揺れる範囲のイージング
	shakeRangeAnim_.Update();
	// カメラの揺れ
	hitShake_->Update();

	// 被弾演出中じゃないならreturn
	if (!player_->GetIsHit()) { return; }

	shakeRangeAnim_.SetIsStart(true);
	hitShake_->SetIsActive(true);

	// 画面の揺れを行う
	if (hitShake_->GetIsEnd()) {
		hitShake_->Reset();
		shakeRangeAnim_.ResetData();
	}
}

void FollowCamera::ShotUpdate() {
	if (multiLockOnSystem_ == nullptr) { return; }

	// カメラの揺れ
	shotShake_->Update();

	// 撃っている最中でないならreturn
	if (!multiLockOnSystem_->GetIsShot()) { return; }

	//shotShake_->SetIsActive(true);

	// 画面の揺れを行う
	if (shotShake_->GetIsEnd()) {
		shotShake_->Reset();
	}
}

Vector3 FollowCamera::TargetOffset() const {
	// 追従対象からのオフセット
	Vector3 offset = targetOffset_ + player_->GetCameraOffset();
	// 回転行列を合成
	Matrix4x4 rotateMatrix = MakeRotateMatrix(camera_->worldTransform_.rotate);

	// オフセットをカメラの回転に合わせて回転
	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}

void FollowCamera::PositionLock(const Vector3& lockTarget) {
	camera_->worldTransform_.translate = lockTarget;
	camera_->Update();
	isLockPos_ = true;
}