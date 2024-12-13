#include "ClearEvent.h"

void ClearEvent::Initialize(Player* player, FollowCamera* followCamera) {
	// インスタンスを取得
	postEffectManager_ = PostEffectManager::GetInstance();

	player_ = player;
	followCamera_ = followCamera;

	// クリア時のUI
	gameClear_ = std::make_unique<Sprite>();
	gameClear_->Initialize("Textures/UI", "gameClear.png");
	gameClear_->SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,-100.0f });
	gameClear_->isActive_ = false;
	postEffectManager_->AddSpriteList(gameClear_.get());

	// 追従対象
	followObject_.Initialize();
	followObject_.translate = player_->GetWorldPosition();
	followObject_.rotate.y = (float)std::numbers::pi;
	followObject_.UpdateMatrix();
	followCamera_->SetTargetOffset(followCamera_->GetTargetOffset());

	// クリア時のUIアニメーション
	gameClearUIAnim_.SetAnimData(&gameClear_->worldTransform_.translate, Vector3{ (float)WinApp::kClientWidth_ / 2,-100.0f,0.0f }, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, 30, Easings::EaseInQuart);
	gameClearUIAnim_.SetAnimData(&gameClear_->worldTransform_.translate, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, 60, Easings::EaseInQuart);
	gameClearUIAnim_.SetAnimData(&gameClear_->worldTransform_.translate, Vector3{ (float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2.0f, 0.0f }, Vector3{ (float)WinApp::kClientWidth_ / 2,-100.0f,0.0f }, 30, Easings::EaseInQuart);

	// クリア時にカメラを自機の後ろに移動するアニメーション
	gameClearCameraOffsetAnim_.SetAnimData(&cameraOffset_, followCamera_->GetTargetOffset(), Vector3{ 0.0f, 0.0f, -10.0f }, 30, Easings::EaseOutQuart);

	// クリア時にカメラを回転させるアニメーション
	gameClearCameraAngleAnim_.SetAnimData(&cameraAngle_, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, (float)std::numbers::pi * 3.0f, 0.0f }, 240, Easings::EaseOutSine);

	velocity_ = { 0,0,1 };
}

void ClearEvent::Update() {
	if (!isActive_) { return; }

	followObject_.UpdateMatrix();

	if (gameClearCameraAngleAnim_.GetIsEnd()) {
		// 自機のクリア挙動
		player_->ClearEffect(isEnd_);
		// 演出がすべて終了したら演出中フラグを切る
		if (isEnd_) { isActive_ = false; }

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
		// 自機座標をカメラに与える
		followObject_.translate = player_->GetWorldPosition();
		followObject_.rotate = cameraAngle_;
	}

	cameraAngle_.x = std::fmod(cameraAngle_.x, (float)std::numbers::pi * 2.0f);
	cameraAngle_.y = std::fmod(cameraAngle_.y, (float)std::numbers::pi * 2.0f);
	cameraAngle_.z = std::fmod(cameraAngle_.z, (float)std::numbers::pi * 2.0f);	

	// 追従カメラの更新
	followCamera_->Update();
	// アニメーション更新
	gameClearUIAnim_.Update();
	gameClearCameraOffsetAnim_.Update();
	gameClearCameraAngleAnim_.Update();

	followCamera_->SetTargetOffset(cameraOffset_);

	followObject_.UpdateMatrix();
}

void ClearEvent::Draw() {

}
