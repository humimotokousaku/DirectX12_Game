#pragma once
#include "IScene.h"

#include "AimAssist/AimAssist.h"
#include "CollisionManager.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "EnemyManager.h"
#include "FollowCamera.h"
#include "PlayerBullet.h"
#include "Player.h"
#include "RailCamera.h"
#include "Score.h"

class GameManager;
class GameScene : public IScene
{
public:
	///
	/// Default Method
	/// 

	GameScene() = default;
	~GameScene()override = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize()override;

	///
	/// User Method
	/// 

	// 自機の弾を追加
	void AddPlayerBullet(PlayerBullet* playerBullet);

private:// プライベートなメンバ変数
	// 基本機能
	TextureManager* textureManager_;
	ModelManager* modelManager_;
	Audio* audio_;

	// BGM
	uint32_t BGM_;

	// カメラ
	Camera camera_;
	// 自機
	Player player_;
	// 自弾
	std::list<PlayerBullet*> playerBullets_;
	// ロックオン機能
	AimAssist* aimAssist_;
	// エネミーマネージャ
	EnemyManager enemyManager_;
	// カメラレール
	RailCamera railCamera_;
	// 追従カメラ
	FollowCamera followCamera_;
	// スコア
	Score* score_;

	// UI
	std::array<Sprite, 4> guideUI_;

	Object3D ground_;
};

