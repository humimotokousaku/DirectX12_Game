#pragma once
#include "RailCamera.h"
#include "Matrix4x4.h"
#include "ImGuiManager.h"
#include "PlayerConfig.h"
#include "PostEffectManager.h"

void RailCamera::Initialize(const std::vector<Vector3>& controlPoints, Player* player) {
	player_ = player;
	controlPoints_ = controlPoints;

	// カメラを生成
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	// 加速中のfovアニメーション
	boostFovAnim_.SetAnimData(&boostFov_, 0.0f, kMaxBoostFovIncrease, 10, Easings::EaseOutBack);
	// 回避中のfovアニメーション
	evasionFovAnim_.SetAnimData(&evasionFov_, 0.0f, kMaxEvasionFovIncrease, 5, Easings::EaseOutExpo);
	// ジャスト回避時のFovアニメーション
	justEvasionFovAnim_.SetAnimData(&justEvasionFov_, 0.0f, kMaxJustEvasionFovIncrease, 6, Easings::EaseOutExpo);

	// カメラの進行度
	t_ = 0.0f;
	// 注視点の進行度
	targetT_ = 1.0f / segmentCount;
	isMove_ = false;
	
	Move();

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

void RailCamera::Update() {
	Move();				// 移動処理
	BoostUpdate();		// 加速中のカメラ処理
	EvasionUpdate();	// 回避中のカメラ処理
	JustEvasionUpdate();// ジャスト回避中のカメラ処理

	// fovを適用
	camera_->viewProjection_.fovAngleY = camera_->kDefaultFov + boostFov_ + evasionFov_ + justEvasionFov_;

	// 行列の更新
	camera_->Update();
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->SetViewMatrix(Inverse(camera_->worldTransform_.matWorld_));

#ifdef _DEBUG
	// ImGui
	ImGuiParameter();
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
	// 注視点を曲線に沿って移動
	target_ = Lerps::CatmullRomSpline(controlPoints_, targetT_);

	// カメラを曲線に沿って移動
	Vector3 cameraPosition = Lerps::CatmullRomSpline(controlPoints_, t_);
	camera_->worldTransform_.translate = cameraPosition + debugVel_;

	// 移動ベクトルを求める
	velocity_ = Subtract(target_, camera_->worldTransform_.translate);
	// 移動ベクトルからY軸周り角度を算出
	camera_->worldTransform_.rotate.y = std::atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	// 移動ベクトルからX軸周りの角度
	camera_->worldTransform_.rotate.x = std::atan2(-velocity_.y, velocityXZ);

	// 移動フラグがないならリターン
	if (!isMove_) { return; }

	// カメラの移動
	if (t_ <= 1.0f) {
		t_ += moveSpeed_;
	}
	// カメラの見ている座標(注視点)を移動
	if (targetT_ <= 1.0f) {
		targetT_ += moveSpeed_;
	}
	// 注視点がゴールまでいったら停止
	if (targetT_ >= 1.0f) {
		currentFrame_ = 0;
		isMove_ = false;
		isGameClear_ = true;
		return;
	}
}

void RailCamera::BoostUpdate() {
	// 加速時はfovを上げる
	if (player_->GetIsBoost()) {
		// 移動速度
		moveSpeed_ = kBoostMoveSpeed * GameTimer::GetInstance()->GetTimeScale();
		// fovアニメーション開始
		boostFovAnim_.SetIsStart(true);
	}
	else {
		// 移動速度
		moveSpeed_ = kNormalMoveSpeed * GameTimer::GetInstance()->GetTimeScale();
		// fov
		boostFov_ = Lerps::ExponentialInterpolate(boostFov_, 0.0f, 0.1f);
		// fovアニメーション終了
		boostFovAnim_.SetIsStart(false);
	}

	// 加速時の視野角のアニメーション
	boostFovAnim_.Update();
}

void RailCamera::EvasionUpdate() {
	// 回避中
	if (player_->GetIsEvasion()) {
		evasionFovAnim_.SetIsStart(true);
	}
	evasionFovAnim_.Update();

	// 回避のアニメーションが終了したとき
	if (evasionFovAnim_.GetIsEnd()) {
		evasionFov_ = Lerps::ExponentialInterpolate(evasionFov_, 0.0f, 0.1f);
	}
}

void RailCamera::JustEvasionUpdate() {
	// ジャスト回避中
	if (player_->GetEvasionData().isJust) {
		// fovを下げる
		justEvasionFovAnim_.SetIsStart(true);
	}
	else {
		justEvasionFovAnim_.SetIsStart(false);
		justEvasionFov_ = Lerps::ExponentialInterpolate(justEvasionFov_, 0.0f, 0.1f);
	}

	justEvasionFovAnim_.Update();
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
	// Fov
	if (ImGui::TreeNode("FovData")) {
		// リセットボタンを作成
		if (ImGui::Button("Regist")) {
			// 加速中のfovアニメーション
			boostFovAnim_.SetFirstAnimData(&boostFov_, 0.0f, kMaxBoostFovIncrease, 10, Easings::EaseOutBack);
			// 回避中のfovアニメーション
			evasionFovAnim_.SetFirstAnimData(&evasionFov_, 0.0f, kMaxEvasionFovIncrease, 5, Easings::EaseOutExpo);
			// ジャスト回避時のFovアニメーション
			justEvasionFovAnim_.SetFirstAnimData(&justEvasionFov_, 0.0f, kMaxJustEvasionFovIncrease, 2, Easings::EaseOutExpo);
		}
		ImGui::TreePop();
	}
	// 速度
	if (ImGui::TreeNode("VelocityData")) {
		ImGui::DragFloat3("Offset", &debugVel_.x, 0.01f, -100, 100);

		if (ImGui::TreeNode("MoveSpeed")) {
			ImGui::DragFloat("Current", &moveSpeed_, 0.0f, -100, 100);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	ImGui::Checkbox("isMove", &isMove_);
	ImGui::Checkbox("isGameClear", &isGameClear_);

	// リセットボタンを作成
	if (ImGui::Button("Reset")) {
		// カメラの進行度をリセット
		Reset();
	}
	ImGui::End();
}