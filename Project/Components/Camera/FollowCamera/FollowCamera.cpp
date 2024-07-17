#include "FollowCamera.h"
#include "Input.h"
#include "ImGuiManager.h"

void FollowCamera::Initialize() {
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
}

void FollowCamera::Update() {
	// 追従対象からカメラまでのオフセット
	Vector3 offset = TargetOffset();

	// 座標をコピーしてオフセット分ずらす
	camera_->worldTransform_.translate = Add(camera_->worldTransform_.parent_->translate, offset);
	camera_->worldTransform_.rotate = camera_->worldTransform_.parent_->rotate;

	camera_->Update();
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->SetViewMatrix(Inverse(camera_->worldTransform_.matWorld_));

#ifdef _DEBUG

	ImGui::Begin("FollowCamera");
	ImGui::DragFloat3("translation", &camera_->worldTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->worldTransform_.rotate.x, 0.1f);
	ImGui::End();

#endif // _DEBUG
}

Vector3 FollowCamera::TargetOffset() const {
	// 追従対象からのオフセット
	Vector3 offset = { 0, 0, -10 };
	offset += (*offset_);
	// 回転行列を合成
	Matrix4x4 rotateMatrix = MakeRotateMatrix(camera_->worldTransform_.parent_->rotate);

	// オフセットをカメラの回転に合わせて回転
	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}