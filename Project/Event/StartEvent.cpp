#include "StartEvent.h"

void StartEvent::Initialize(Player* player, FollowCamera* followCamera, RailCamera* railCamera) {
	player_ = player;
	followCamera_ = followCamera;
	railCamera_ = railCamera;

	isActive_ = true;

	startWorldTransform_.Initialize();
	startWorldTransform_.translate = railCamera_->GetControlPointPos(0) + Normalize(railCamera_->GetDirectionVelocity()) * 250.0f;
	startWorldTransform_.translate.y = railCamera_->GetControlPointPos(0).y - 5.0f;
	startWorldTransform_.UpdateMatrix();
	followCamera_->SetParent(&startWorldTransform_);

	// ステージ開始演出のカメラアングル
	startAngleAnim_[0].SetAnimData(&startWorldTransform_.rotate, Vector3{ 0.0f,0.0f, 0.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, 180, Easings::EaseOutCubic);
	startAngleAnim_[0].SetIsStart(true); 
}

void StartEvent::Update() {
	if (!isActive_) { return; }

	// 追従カメラの座標を固定する
	followCamera_->PositionLock(startWorldTransform_.translate);

	// 自機のいるところにカメラを向ける
	Vector3 toPlayer = player_->GetWorldPosition() - startWorldTransform_.GetWorldPosition();
	toPlayer = Normalize(toPlayer);
	velocity_ = Normalize(velocity_);
	velocity_ = Lerps::Slerp(velocity_, toPlayer, 1.0f) * GameTimer::GetInstance()->GetTimeScale();
	// Y軸周り角度(θy)
	startWorldTransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ;
	velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	// X軸周りの角度(θx)
	startWorldTransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);
	startWorldTransform_.UpdateMatrix();

	player_->StartStageEffect(railCamera_->GetDirectionVelocity() * -1.0f);
	// 演出がすべて終了したら演出中フラグを切る
	if (isEnd_) { isActive_ = false; }

	// 追従カメラの更新
	followCamera_->Update();

	startWorldTransform_.UpdateMatrix();

	for (Animation& startAnim : startAngleAnim_) {
		startAnim.Update();
	}

	// アニメーションが終了したらゲーム開始
	if (startAngleAnim_[0].GetIsEnd()) {
		isActive_ = false;
		isEnd_ = true;
		followCamera_->SetIsLockPos(false);
	}
}