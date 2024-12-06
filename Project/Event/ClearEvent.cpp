#include "ClearEvent.h"

void ClearEvent::Initialize(Player* player, FollowCamera* followCamera) {
	// インスタンスを取得
	postEffectManager_ = PostEffectManager::GetInstance();

	player_ = player;
	followCamera_ = followCamera;

	gameClear_ = std::make_unique<Sprite>();
	gameClear_->Initialize("Textures/UI", "gameClear.png");
	gameClear_->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 });
	gameClear_->isActive_ = false;
	postEffectManager_->AddSpriteList(gameClear_.get());

	// 追従対象
	followObject_.Initialize();
	followObject_.translate = player_->GetWorldPosition();
	followObject_.UpdateMatrix();
	followCamera_->SetParent(&followObject_);

	// クリア時のUIアニメーション
	gameClearUIAnim_.SetAnimData(&gameClear_->worldTransform_.translate, Vector3{ 0.0f,-100.0f,0.0f }, Vector3{ 0.0f, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, 30, Easings::EaseInQuart);
	gameClearUIAnim_.SetAnimData(&gameClear_->worldTransform_.translate, Vector3{ 0.0f, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, Vector3{ 0.0f, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, 60, Easings::EaseInQuart);
	gameClearUIAnim_.SetAnimData(&gameClear_->worldTransform_.translate, Vector3{ 0.0f, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, Vector3{ 0.0f,-100.0f,0.0f }, 30, Easings::EaseInQuart);

	// クリア時にカメラを自機の後ろに移動するアニメーション
	gameClearCameraOffsetAnim_.SetAnimData(&cameraOffset_, followCamera_->GetTargetOffset() + player_->GetCameraOffset(), Vector3{ 0.0f, 0.0f, -15.0f }, 30, Easings::EaseOutQuart);
}

void ClearEvent::Update() {
	if (gameClearCameraOffsetAnim_.GetIsEnd()) {
		// 自機のいるところにカメラを向ける
		Vector3 toPlayer = player_->GetWorldPosition() - followObject_.GetWorldPosition();
		toPlayer = Normalize(toPlayer);
		velocity_ = Normalize(velocity_);
		velocity_ = Lerps::Slerp(velocity_, toPlayer, 1.0f) * GameTimer::GetInstance()->GetTimeScale();
		// Y軸周り角度(θy)
		followObject_.rotate.y = std::atan2(velocity_.x, velocity_.z);
		// 横軸方向の長さを求める
		float velocityXZ;
		velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
		// X軸周りの角度(θx)
		followObject_.rotate.x = std::atan2(-velocity_.y, velocityXZ);
		followObject_.UpdateMatrix();
	}
	else {
		// 自機にカメラを追従させる
		followObject_.translate = player_->GetWorldPosition();
		followObject_.rotate.y += 0.01f;
		followObject_.UpdateMatrix();
	}
}

void ClearEvent::Draw() {

}
