#pragma once
#include "AimAssist/AimAssist.h"
#include "CollisionManager.h"
#include "LevelManager.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "EnemyManager.h"
#include "FollowCamera.h"
#include "PlayerBullet.h"
#include "Player.h"
#include "RailCamera.h"
#include "Score.h"
#include "Skydome.h"
#include "MultiLockOnSystem.h"

/// <summary>
/// ゲームシーンで行われる処理をまとめたクラス
/// </summary>
class GameSystem {
public:
	GameSystem() = default;
	~GameSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="sceneNum">シーン番号</param>
	void Update(int& sceneNum);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	///
	/// User Method
	/// 

	// 自機の弾を追加
	void AddPlayerBullet(PlayerBullet* playerBullet) { playerBullets_.push_back(playerBullet); }

private:
	/// <summary>
	/// シーン切り替えの条件
	/// </summary>
	/// <param name="sceneNum">シーン番号</param>
	void SceneChange(int& sceneNum);

	// モデル追加
	void AddModel(Model* model) { models_.push_back(model); }

private:// エンジン機能
	TextureManager* textureManager_;
	ModelManager* modelManager_;
	Audio* audio_;
	LevelManager* levelManager_;

private:// プライベートなメンバ変数
	// 登録用のモデル
	std::vector<Model*> models_;
	// ロックオン機能
	AimAssist* aimAssist_;
	// スコア
	Score* score_;

	// エネミーマネージャ
	EnemyManager enemyManager_;

	// 自機
	Player player_;
	// 自弾
	std::list<PlayerBullet*> playerBullets_;
	// マルチロックオン機能
	std::unique_ptr<MultiLockOnSystem> multiLockOnSystem_;

	// カメラ
	Camera camera_;
	// カメラレール
	RailCamera railCamera_;
	// 追従カメラ
	FollowCamera followCamera_;

	// 天球
	Skydome skydome_;
	// UI
	std::array<Sprite, 4> guideUI_;

	// BGM
	uint32_t BGM_;
};