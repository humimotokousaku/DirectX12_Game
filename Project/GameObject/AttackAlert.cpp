#include "AttackAlert.h"

AttackAlert* AttackAlert::GetInstance() {
	static AttackAlert instance;
	return &instance;
}

void AttackAlert::Initialize() {
	postEffectManager_ = PostEffectManager::GetInstance();

	// UIのスプライトを作成
	alertSprite_[0].Initialize("Textures/UI", "alertBelt.png");
	alertSprite_[0].SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 });
	alertSprite_[1].Initialize("Textures/UI", "guide_alert.png");
	alertSprite_[1].SetPos(Vector2{ (float)WinApp::kClientWidth_ / 2,(float)WinApp::kClientHeight_ / 4 });
	for (int i = 0; i < alertSprite_.size(); i++) {
		alertSprite_[i].isActive_ = false;
		postEffectManager_->AddSpriteList(&alertSprite_[i]);
	}
	// 2Dレティクル作成
	sprite2DReticle_[0].Initialize("", "reticle.png");
	sprite2DReticle_[0].SetSize(Vector2{ 50.0f,50.0f });
	sprite2DReticle_[1].Initialize("", "lockOnReticle.png");
	for (int i = 0; i < sprite2DReticle_.size(); i++) {
		sprite2DReticle_[i].isActive_ = false;
		sprite2DReticle_[i].SetPos(Vector2((float)WinApp::kClientWidth_ / 2, (float)WinApp::kClientHeight_ / 2));
		PostEffectManager::GetInstance()->AddSpriteList(&sprite2DReticle_[i]);
	}

	// アラートのアニメーション
	alertAnim_[0].SetAnimData(alertSprite_[0].GetSizeP(), Vector2{ 1280.0f,0.0f }, Vector2{ 1280.0f,88.0f }, 5, "s", Easings::EaseOutExpo);
	alertAnim_[1].SetAnimData(alertSprite_[1].GetSizeP(), Vector2{ 370.0f,0.0f }, Vector2{ 370.0f,64.0f }, 5, "s", Easings::EaseOutExpo);

	// ロックオン時のレティクルのアニメーション
	reticleAnim_.SetAnimData(sprite2DReticle_[1].GetSizeP(), Vector2{ 256,256 }, Vector2{ 86,86 }, 8, "LockOnReticle_size_Anim", Easings::EaseInExpo);

	// ロックオン対象の座標
	targetPos_ = { 0,0,0 };

	isStart_ = false;
	isLockOn_ = false;
	currentFrame_ = kFinishAlert;
}

void AttackAlert::Update() {
	if (!isStart_ && !isLockOn_) { return; }

	// ロックオン処理
	LockOnReticle();

	// uvTransformを動かす
	alertSprite_[0].uvTransform_.translate.x += 0.05f;

	// アニメーションの更新
	for (int i = 0; i < alertAnim_.size(); i++) {
		alertAnim_[i].Update();
	}
	reticleAnim_.Update();

	// 既定の時間が経過したら終了
	if (currentFrame_ <= 0) {
		// パラメータの初期化
		End();
	}

	currentFrame_--;
}

void AttackAlert::LockOnReticle() {
	// ロックオン時間中
	if (currentFrame_ >= kFinishAlert - kLockOnFrame) { 
		// 自機のワールド座標を取得
		targetPos_ = player_->GetWorldPosition();;
	}

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport{};
	matViewProjectionViewport =
		Multiply(camera_->GetViewProjection().matView, Multiply(camera_->GetViewProjection().matProjection, matViewport));
	// ワールド→スクリーン座標変換
	Vector3 positionReticle{};
	positionReticle = Transforms(targetPos_, matViewProjectionViewport);
	// スプライトのレティクルに座標を設定
	sprite2DReticle_[0].SetPos(Vector2(positionReticle.x, positionReticle.y));
	// ロックオンレティクルにも代入
	sprite2DReticle_[1].SetPos(sprite2DReticle_[0].GetPos());
}