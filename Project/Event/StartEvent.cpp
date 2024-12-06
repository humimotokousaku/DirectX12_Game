#include "StartEvent.h"

void StartEvent::Initialize(Player* player, FollowCamera* followCamera) {
	player_ = player;
	followCamera_ = followCamera;

	isActive_ = true;

	player_->Update();

	startWorldTransform_.Initialize();
	startWorldTransform_.translate = player_->GetWorldPosition();
	startWorldTransform_.UpdateMatrix();
	followCamera_->SetParent(&startWorldTransform_);


	// ステージ開始演出のカメラアングル
	startAngleAnim_[0].SetAnimData(&startWorldTransform_.rotate, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, 60, Easings::EaseOutCubic);
	startAngleAnim_[0].SetAnimData(&startWorldTransform_.rotate, Vector3{ 0.0f,0.0f,0.0f }, Vector3{ 0.0f,(float)std::numbers::pi / 2,0.0f }, 300, Easings::EaseInCubic);
	startAngleAnim_[0].SetAnimData(&startWorldTransform_.rotate, Vector3{ 0.0f,(float)std::numbers::pi / 2,0.0f }, Vector3{ 0.0f,(float)std::numbers::pi,0.0f }, 300, Easings::EaseOutCubic);
	startAngleAnim_[0].SetIsStart(true);
	// カメラのオフセット
	startAngleAnim_[1].SetAnimData(&cameraOffset_, Vector3{ 0.0f,1.0f,-5.0f }, Vector3{ 0.0f,1.0f,-5.0f }, 60, Easings::EaseOutCubic);
	startAngleAnim_[1].SetAnimData(&cameraOffset_, Vector3{ 0.0f,1.0f,-5.0f }, Vector3{ 0.0f,5,-20.0f }, 600, Easings::EaseInCubic);
	startAngleAnim_[1].SetIsStart(true);
}

void StartEvent::Update() {
	if (!isActive_) { return; }

	followCamera_->SetTargetOffset(cameraOffset_);
	followCamera_->Update();

	startWorldTransform_.translate = player_->GetWorldPosition();
	startWorldTransform_.UpdateMatrix();

	for (Animation& startAnim : startAngleAnim_) {
		startAnim.Update();
	}

	// アニメーションが終了したらゲーム開始
	if (startAngleAnim_[0].GetIsEnd()) {
		isActive_ = false;
	}
}