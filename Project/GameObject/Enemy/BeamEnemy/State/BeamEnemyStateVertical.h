#pragma once
#include "IBeamEnemyState.h"

class BeamEnemy;
/// <summary>
/// ビームを撃つ敵の攻撃状態
/// </summary>
class BeamEnemyStateVertical : public IBeamEnemyState {
public:
	BeamEnemyStateVertical(BeamEnemy* enemy, Player* player);
	~BeamEnemyStateVertical()override = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

public:

};