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

	t_ = 0.0f;
	targetT_ = 1.0f / segmentCount;
	isMoveCamera_ = true;
}
void RailCamera::Initialize(std::vector<Vector3> controlPoints, Camera* camera) {
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
		line_[i]->SetCamera(camera);
		line_[i]->startPos_ = pointsDrawing_[i];
		line_[i]->endPos_ = pointsDrawing_[i + 1];
	}

	t_ = 0.0f;
	targetT_ = 1.0f / segmentCount;
	isMoveCamera_ = true;
}

void RailCamera::Update() {
	if (isMoveCamera_) {
		// カメラの移動
		if (t_ <= 1.0f) {
			t_ += 1.0f / segmentCount / 10;
		}
		// カメラの見ている座標を移動
		if (targetT_ <= 1.0f) {
			targetT_ += 1.0f / segmentCount / 10;
		}
		if (targetT_ >= 1.0f) {
			targetT_ = 0.999f;
			t_ = 0.98f;
			isMoveCamera_ = false;
		}
	}
	target_ = Lerps::CatmullRomSpline(controlPoints_, targetT_);
	//target_.y += 0.2f;
	Vector3 cameraPosition{};
	// Catmull-Romスプライン関数で補間された位置を取得
	cameraPosition = Lerps::CatmullRomSpline(controlPoints_, t_);
	//cameraPosition.y += 0.2f;
	camera_->worldTransform_.translate = cameraPosition;


	velocity_ = Subtract(target_, camera_->worldTransform_.translate);
	// Y軸周り角度(θy)
	camera_->worldTransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ;
	velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	// X軸周りの角度(θx)
	camera_->worldTransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);
	// 行列の更新
	camera_->Update();

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->SetViewMatrix(Inverse(camera_->worldTransform_.matWorld_));

//#ifdef _DEBUG

	ImGui::Begin("Camera");
	ImGui::DragFloat3("translation", &camera_->worldTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->worldTransform_.rotate.x, 0.1f);
	ImGui::End();

//#endif // _DEBUG
}

void RailCamera::MoveRouteDraw() {
	for (int i = 0; i < segmentCount; i++) {
		line_[i]->Draw();
	}
}
