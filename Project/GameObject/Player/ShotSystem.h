#pragma once
#include "Player.h"
#include "PlayerBullet.h"
#include "PlayerConfig.h"

class GameSystem;
class ShotSystem {
//public:
//	struct LockOnData {
//		WorldTransform* enemyData;
//		bool isActive;
//	};

public:
	void Initialize(Player* player, Camera* camera, std::list<IEnemy*> enemys, GameSystem* gameSystem, Model* model);
	void Update();

	void Attack();

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	// 全敵のアドレスを設定
	void SetEnemyList(std::list<IEnemy*> enemys) { enemys_ = enemys; }
	// マルチロックオンクラスのアドレスを設定
	//void SetMultiLockOnSystem(std::vector<MultiLockOnData> lockOnDatas) { multiLockOnDatas_ = lockOnDatas; }

private:
	// 自機
	Player* player_;
	// 弾
	std::list<PlayerBullet*> bullets_;
	// 全ての敵
	std::list<IEnemy*> enemys_;
	// カメラ
	Camera* camera_;
	// ゲームシステム
	GameSystem* gameSystem_;

	// 弾の見た目
	Model* model_;

	// ロックオン中の敵データ
	//std::vector<MultiLockOnData> multiLockOnDatas_;

	//std::vector<LockOnData> lockOnDatas_;
};