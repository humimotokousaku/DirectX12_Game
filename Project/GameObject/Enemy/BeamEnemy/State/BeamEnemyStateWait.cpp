#include "BeamEnemyStateWait.h"
#include "BeamEnemyStateVertical.h"
#include "BeamEnemyStateHorizontal.h"
#include "BeamEnemy.h"

BeamEnemyStateWait::BeamEnemyStateWait(BeamEnemy* enemy, Player* player) {
	enemy_ = enemy;
	player_ = player;
}

void BeamEnemyStateWait::Initialize() {

}

void BeamEnemyStateWait::Update() {
	enemy_->SetRotate(Vector3{ 0,0,0 });
	enemy_->ChangeState(new BeamEnemyStateVertical(enemy_, player_));
}