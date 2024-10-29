#pragma once
#include "RailCamera.h"
#include "../math/Matrix4x4.h"
#include "../Manager/ImGuiManager.h"

void RailCamera::Initialize(std::vector<Vector3> controlPoints, Player* player) {
	player_ = player;
	controlPoints_ = controlPoints;

	// カメラを生成
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	// 加速中のfovアニメーション
	boostFovAnim_.SetAnimData(&boostFov_, 0.0f, kMaxBoostFovIncrease, 10, "fovAnim_01", Easings::EaseOutBack);
	// 回避中のfovアニメーション
	evasionFovAnim_.SetAnimData(&evasionFov_, 0.0f, kMaxEvasionFovIncrease, 5, "fovAnim_01", Easings::EaseOutExpo);

	t_ = 0.0f;
	targetT_ = 1.0f / segmentCount;
	isMove_ = true;

	// アドレスを代入
	isBoost_ = player_->GetIsBoost_P();

#pragma region デバッグ用
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
#pragma endregion
}
static float moveSpeed = 0.2f;
void RailCamera::Update() {
	Move();			// 移動処理
	BoostUpdate();	// 加速中のカメラ処理
	EvasionUpdate();// 回避中のカメラ処理

	// fovを適用
	camera_->viewProjection_.fovAngleY = camera_->kDefaultFov + boostFov_ + evasionFov_;

	// 行列の更新
	camera_->Update();
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->SetViewMatrix(Inverse(camera_->worldTransform_.matWorld_));
	// ImGui
	ImGuiParameter();
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

void RailCamera::Move() {
	// 移動フラグがないなら早期リターン
	if (!isMove_) { return; }

	// カメラの移動
	if (t_ <= 1.0f) {
		t_ += moveSpeed / 1000;
	}
	// カメラの見ている座標(注視点)を移動
	if (targetT_ <= 1.0f) {
		targetT_ += moveSpeed / 1000;
	}
	// 注視点がゴールまでいったら停止
	if (targetT_ >= 1.0f) {
		currentFrame_ = 0;
		isMove_ = false;
		isGameClear_ = true;
		return;
	}

	// 注視点を曲線に沿って移動
	target_ = Lerps::CatmullRomSpline(controlPoints_, targetT_);
	Vector3 cameraPosition{};
	// カメラを曲線に沿って移動
	cameraPosition = Lerps::CatmullRomSpline(controlPoints_, t_);
	camera_->worldTransform_.translate = cameraPosition + debugVel_;

	// 移動ベクトルを求める
	velocity_ = Subtract(target_, camera_->worldTransform_.translate);
	// 移動ベクトルからY軸周り角度を算出
	camera_->worldTransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	// 移動ベクトルからX軸周りの角度
	camera_->worldTransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);
}

void RailCamera::BoostUpdate() {
	// 加速時はfovを上げる
	if (*isBoost_) {
		moveSpeed = 0.8f;
		// fov
		boostFovAnim_.SetIsStart(true);
	}
	else {
		moveSpeed = 0.6f;
		// fov
		boostFov_ = Lerps::ExponentialInterpolate(boostFov_, 0.0f, 1.0f, 0.1f);
		boostFovAnim_.SetIsStart(false);
	}
	boostFovAnim_.Update();
}

void RailCamera::EvasionUpdate() {
	if (player_->GetIsEvasion()) {
		evasionFovAnim_.SetIsStart(true);
	}
	evasionFovAnim_.Update();

	if (evasionFovAnim_.GetIsEnd()) {
		evasionFov_ = Lerps::ExponentialInterpolate(evasionFov_, 0.0f, 1.0f, 0.1f);
	}
}

void RailCamera::Reset() {
	t_ = 0.0f;
	targetT_ = 1.0f / segmentCount;
	isMove_ = true;
}

void RailCamera::ImGuiParameter() {
	ImGui::Begin("RailCamera");
	ImGui::DragFloat3("translation", &camera_->worldTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_->worldTransform_.rotate.x, 0.1f);
	ImGui::DragFloat("fov", &camera_->viewProjection_.fovAngleY, 0.1f, 0, 200);
	ImGui::Checkbox("isMove", &isMove_);
	ImGui::DragFloat3("vel", &debugVel_.x, 0.01f, -100, 100);
	ImGui::DragFloat("moveSpeed", &moveSpeed, 0.1f, -100, 100);
	// リセットボタンを作成
	if (ImGui::Button("Reset")) {
		// カメラの進行度をリセット
		Reset();
	}
	ImGui::End();
}