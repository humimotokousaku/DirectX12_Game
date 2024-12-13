#include "MoveEnemyStateLeave.h"
#include "MoveEnemy.h"
#include "Vector3.h"

void MoveEnemyStateLeave::Initialize(MoveEnemy* enemy, Player* player) {
	enemy_ = enemy; 
	player_ = player;
}

void MoveEnemyStateLeave::Update(MoveEnemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = { -0.05f, 0.05f, -0.05f };

	// 移動処理
	enemy->Move(kMoveSpeed);
}