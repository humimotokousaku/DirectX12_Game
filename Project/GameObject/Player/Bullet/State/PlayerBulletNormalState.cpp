#include "PlayerBulletNormalState.h"
#include "PlayerBulletFollowState.h"
#include "Player.h"
#include "PlayerBullet.h"

PlayerBulletNormalState::PlayerBulletNormalState(PlayerBullet* playerBullet, Player* player, const Vector3& shotAngle) {
	playerBullet_ = playerBullet;
	player_ = player;

	// ホーミング弾に遷移するまでの時間
	currentFrame_ = kChangeStateInterval;

	// 発射角度から速度を算出
	Vector3 velocity = { 0,0,1 };
	velocity_ = Normalize(TransformNormal(velocity, MakeRotateMatrix(shotAngle))) * kBulletSpeed;

	// 弾の角度を設定
	playerBullet_->SetRotate(shotAngle);

	// 弾の速度
	velocityAnim_.SetAnimData(&velocity_, Vector3{ 0,0,0 }, velocity_, 6, Easings::EaseOutExpo);
	velocityAnim_.SetAnimData(&velocity_, velocity_, Vector3{ 0,0,0 },10, Easings::EaseOutExpo);
	velocityAnim_.SetIsStart(true);
}

void PlayerBulletNormalState::Initialize() {

}

void PlayerBulletNormalState::Update() {
	// 速度を加算
	currentPos_ += velocity_;

	// 速度を変化させる
	velocityAnim_.Update();

	// playerBulletクラスで座標が変化しないようにする
	playerBullet_->SetVelocity(Vector3{0,0,0});
	// ホーミング弾でないときは自機に追従
	playerBullet_->SetPos(player_->GetWorldPosition() + currentPos_);

	// 経過時間を更新
	currentFrame_-=GameTimer::GetInstance()->GetTimeScale();
	
	// 一定時間経過でホーミング弾になる
	if (currentFrame_ <= 0) {
		currentFrame_ = kChangeStateInterval;
		playerBullet_->ChangeState(new PlayerBulletFollowState(playerBullet_,player_));
	}
}