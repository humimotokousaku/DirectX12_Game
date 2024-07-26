#pragma once
#include "IScene.h"
#include "EnemyBullet.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "PlayerBullet.h"
#include "Player.h"
#include "AimAssist/AimAssist.h"
#include "RailCamera.h"
#include "FollowCamera.h"
#include "Cube.h"
#include "CollisionManager.h"

class GameManager;
class GameScene : public IScene
{
public:
	///
	/// Default Method
	/// 

	GameScene();
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

	// カメラ
	std::unique_ptr<Camera> camera_;
	// 自機
	std::unique_ptr<Player> player_;
	// 自弾
	std::list<PlayerBullet*> playerBullets_;
	// ロックオン機能
	AimAssist* aimAssist_;
	// エネミーマネージャ
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;
	// 衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;
	// カメラレール
	std::unique_ptr<RailCamera> railCamera_ = nullptr;
	// 追従カメラ
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;
	// skybox
	std::unique_ptr<Cube> cube_;
	// UI
	std::array<std::unique_ptr<Sprite>, 2> guideUI_;
};

