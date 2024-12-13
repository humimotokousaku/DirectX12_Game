#pragma once
#include "IBeamEnemyState.h"

class BeamEnemy;
/// <summary>
/// ビームを撃つ敵の待機状態
/// </summary>
class BeamEnemyStateWait : public IBeamEnemyState {
public:
	BeamEnemyStateWait(BeamEnemy* enemy, Player* player);
	~BeamEnemyStateWait()override = default;

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