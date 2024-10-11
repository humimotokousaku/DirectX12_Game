#pragma once
#include "RailCamera.h"
#include "../math/Matrix4x4.h"
#include "../Manager/ImGuiManager.h"

void RailCamera::Initialize(std::vector<Vector3> controlPoints) {
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	controlPoints_ = controlPoints;

	// 線分の数+1個分の頂点座標の計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		if (t >= 1.0f) {
			t = 0.99f;
		}
		Vector3 pos = Lerps::CatmullRomSpline(controlPoints_, t);
		// 描画用頂点リストに追加
		pointsDrawing_.push_back(pos);
	}
	// 線
	for (int i = 0; i < segmentCount; i++) {
		line_[i] = std::make_unique<Line>();
		line_[i]->Initialize();
		line_[i]->SetCamera(camera_.get());
		line_[i]->startPos_ = pointsDrawing_[i];
		line_[i]->endPos_ = pointsDrawing_[i + 1];
	}

	// 注視点
	sphere_.Initialize();
	sphere_.SetCamera(camera_.get());
	sphere_.worldTransform.scale = { 0.5f,0.5f,0.5f };
	sphere_.worldTransform.UpdateMatrix();

	t_ = 0.0f;
	targetT_ = 1.0f / segmentCount;
	isMove_ = true;

	boostFovAnim_.SetAnimData(&camera_->viewProjection_.fovAngleY, camera_->kDefaultFov, 70.0f, 10, "fovAnim_01", Easings::EaseOutBack);
}

void RailCamera::Update() {
	// 加速中のカメラ処理
	BoostUpdate();

	if (isMove_) {
		// カメラの移動
		if (t_ <= 1.0f) {
			t_ += *moveVelZ_  / 1000;
		}
		// カメラの見ている座標を移動
		if (targetT_ <= 1.0f) {
			targetT_ += *moveVelZ_ / 1000;
		}
		if (targetT_ >= 1.0f) {
			currentFrame_ = 0;
			isMove_ = false;
			isGameClear_ = true;
		}
	}
	target_ = Lerps::CatmullRomSpline(controlPoints_, targetT_);
	Vector3 cameraPosition{};
	// Catmull-Romスプライン関数で補間された位置を取得
	cameraPosition = Lerps::CatmullRomSpline(controlPoints_, t_);
	camera_->worldTransform_.translate = cameraPosition + debugVel_;

	velocity_ = Subtract(target_, camera_->worldTransform_.translate);
	// Y軸周り角度(θy)
	camera_->worldTransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	// X軸周りの角度(θx)
	camera_->worldTransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);

	// 行列の更新
	camera_->Update();
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->SetViewMatrix(Inverse(camera_->worldTransform_.matWorld_));

	ImGui::Begin("RailCamera");
	ImGui::DragFloat3("translation", &camera_->worldTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->worldTransform_.rotate.x, 0.1f);
	ImGui::DragFloat("fov", &camera_->viewProjection_.fovAngleY, 0.1f, 0,200);
	ImGui::Checkbox("isMove", &isMove_);
	ImGui::DragFloat3("vel", &debugVel_.x, 0.01f, -100,100);
	ImGui::End();
#ifdef _DEBUG
	// デバッグ用のカメラの注視点の座標を更新
	sphere_.worldTransform.translate = target_;
	sphere_.worldTransform.UpdateMatrix();
#endif // _DEBUG
}

void RailCamera::MoveRouteDraw() {
#ifdef _DEBUG
	for (int i = 0; i < segmentCount; i++) {
		line_[i]->Draw();
	}
	sphere_.Draw(kFillModeWireFrame);
#endif // _DEBUG
}

void RailCamera::BoostUpdate() {
	// 加速時はfovを上げる
	if (*isBoost_) {
		// fov
		boostFovAnim_.SetIsStart(true);
		//camera_->viewProjection_.fovAngleY = Lerps::ExponentialInterpolate(camera_->viewProjection_.fovAngleY, 60, 1.0f, 0.9f);
		//moveVel_ = Lerps::ExponentialInterpolate(moveVel_, 2, 1.0f, 0.1f);
	}
	else {
		// fov
		camera_->viewProjection_.fovAngleY = Lerps::ExponentialInterpolate(camera_->viewProjection_.fovAngleY, camera_->kDefaultFov, 1.0f, 0.1f);
		boostFovAnim_.SetIsStart(false);
		//moveVel_ = Lerps::ExponentialInterpolate(moveVel_, 0, 1.0f, 0.1f);
	}
	boostFovAnim_.Update();
}
