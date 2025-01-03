#include "PlayerBulletFollowState.h"
#include "Player.h"
#include "PlayerBullet.h"

PlayerBulletFollowState::PlayerBulletFollowState(PlayerBullet* playerBullet, Player* player) {
	playerBullet_ = playerBullet;
	player_ = player;
}

void PlayerBulletFollowState::Initialize() {

}

void PlayerBulletFollowState::Update() {
	Vector3 toPlayer = playerBullet_->GetEnemyData()->GetWorldPosition() - playerBullet_->GetWorldPosition();
	toPlayer = Normalize(toPlayer);
	playerBullet_->SetVelocity(Normalize(playerBullet_->GetVelocity()));
	playerBullet_->SetVelocity(Lerps::Slerp(playerBullet_->GetVelocity(), toPlayer, followRate_) * playerBullet_->GetBulletSpeed() * GameTimer::GetInstance()->GetTimeScale());

	followRate_ = Lerps::ExponentialInterpolate(followRate_, 1.0f, 1.0f);
}