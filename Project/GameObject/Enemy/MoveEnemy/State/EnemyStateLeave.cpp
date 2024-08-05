#include "EnemyStateLeave.h"
#include "Enemy.h"
#include "../math/Vector3.h"

void EnemyStateLeave::Initialize(Enemy* enemy, Player* player) { 
	enemy_ = enemy; 
	player_ = player;
}

void EnemyStateLeave::Update(Enemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = { -0.05f, 0.05f, -0.05f };

	// 移動処理
	enemy->Move(kMoveSpeed);
}