#include "PlayerBulletNormalState.h"
#include "PlayerBulletFollowState.h"
#include "Player.h"
#include "PlayerBullet.h"

PlayerBulletNormalState::PlayerBulletNormalState(PlayerBullet* playerBullet, Player* player) {
	playerBullet_ = playerBullet;
	player_ = player;

	currentFrame_ = kChangeStateInterval;
}

void PlayerBulletNormalState::Initialize() {

}

void PlayerBulletNormalState::Update() {
	// 弾の速度
	const float kBulletSpeed = -15.0f;
	Vector3 velocity = kBulletVelocity * kBulletSpeed;
	
	playerBullet_->SetVelocity(velocity);

	currentFrame_--;
	
	if (currentFrame_ <= 0) {
		currentFrame_ = kChangeStateInterval;
		playerBullet_->ChangeState(new PlayerBulletFollowState(playerBullet_,player_));
	}
}