#include "FollowCamera.h"
#include "Input.h"
#include "ImGuiManager.h"

void FollowCamera::Initialize(Player* player) {
	player_ = player;

	// カメラの生成
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	// 被弾時の画面揺れ
	hitShake_ = new Shake();
	hitShake_->SetRange(Vector3{ 2.0f,2.0f,0.0f });
	hitShake_->SetTarget(&shakeOffset_);
	hitShake_->SetEndFrame(30.0f);
	// 被弾時のカメラの揺れる範囲
	const float endFrame = 5;
	const Vector3 end = { 0.3f, 0.3f,0.0f };
	shakeRangeAnim_.SetAnimData(hitShake_->GetRange_P(), hitShake_->GetRange(), end, endFrame, Easings::EaseInExpo);
	shakeRangeAnim_.SetAnimData(hitShake_->GetRange_P(), end, Vector3{ 0,0,0 }, hitShake_->GetEndFrame() - endFrame, Easings::EaseInCubic);
}

void FollowCamera::Update() {
	// 被弾演出
	HitUpdate();

	// 親のオイラー角を取得
	camera_->worldTransform_.rotate = camera_->worldTransform_.parent_->rotate;

	// 追従対象からカメラまでのオフセット
	Vector3 offset = TargetOffset();

	// 座標をコピーしてオフセット分ずらす
	camera_->worldTransform_.translate = camera_->worldTransform_.parent_->translate + offset + shakeOffset_;
	// 演出用のoffsetを加算
	camera_->worldTransform_.rotate += player_->GetCameraRotateOffset();

	// fov
	camera_->viewProjection_.fovAngleY = *fov_;
	// カメラの更新
	camera_->Update();
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->SetViewMatrix(Inverse(camera_->worldTransform_.matWorld_));

#ifdef _DEBUG
	// ImGui
	ImGui::Begin("FollowCamera");
	ImGui::DragFloat3("translation", &camera_->worldTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->worldTransform_.rotate.x, 0.1f);	
	ImGui::DragFloat("fov", &camera_->viewProjection_.fovAngleY, 0.1f, 0, 200);
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

	// 画面の揺れを行う
	if (hitShake_->GetIsEnd()) {
		hitShake_->Reset();
		shakeRangeAnim_.ResetData();
	}

	shakeRangeAnim_.SetIsStart(true);
	hitShake_->SetIsActive(true);

	// 画面の揺れを行う
	if (hitShake_->GetIsEnd()) {
		hitShake_->Reset();
		shakeRangeAnim_.ResetData();
		//input_->GamePadVibration(0, 0, 0);
	}
}

Vector3 FollowCamera::TargetOffset() const {
	// 追従対象からのオフセット
	Vector3 offset = { 0, 5, -20 };
	offset += player_->GetCameraOffset();
	// 回転行列を合成
	Matrix4x4 rotateMatrix = MakeRotateMatrix(camera_->worldTransform_.rotate);

	// オフセットをカメラの回転に合わせて回転
	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}