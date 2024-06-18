#pragma once
#include "RailCamera.h"
#include "../math/Matrix4x4.h"
#include "../Manager/ImGuiManager.h"

void RailCamera::Initialize() {
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	//camera_->worldTransform_.translate.x = 5;
	//camera_->worldTransform_.translate.y = 5;
	//camera_->worldTransform_.translate.z = -30;
}

void RailCamera::Update(Vector3 target) {
	Vector3 velocity = Subtract(target, camera_->worldTransform_.translate);
	// Y軸周り角度(θy)
	camera_->worldTransform_.rotate.y = std::atan2(velocity.x, velocity.z);
	// 横軸方向の長さを求める
	float velocityXZ;
	velocityXZ = sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	// X軸周りの角度(θx)
	camera_->worldTransform_.rotate.x = std::atan2(-velocity.y, velocityXZ);
	// 行列の更新
	//worldTransform_.UpdateMatrix();
	camera_->Update();

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->SetViewMatrix(Inverse(camera_->worldTransform_.matWorld_));

#ifdef _DEBUG

	ImGui::Begin("Camera");
	ImGui::DragFloat3("translation", &camera_->worldTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->worldTransform_.rotate.x, 0.1f);
	ImGui::End();

#endif // _DEBUG
}