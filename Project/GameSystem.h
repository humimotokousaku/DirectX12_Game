#pragma once
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
#include "Skydome/Skydome.h"
#include "MultiLockOnSystem.h"
#include "StartEvent.h"
#include "ClearEvent.h"
#include "DeadEvent.h"

/// <summary>
/// ゲームシーンで行われる処理をまとめたクラス
/// </summary>
class GameSystem {
public:
	// 演出の状態
	enum EffectState {
		Normal,
		FirstJustEvasion,
		JustEvasion,
		Boost
	};

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
	// 自機の弾リストを取得
	std::list<PlayerBullet*> GetPlayerBulletList() { return playerBullets_; }

	/// <summary>
	/// 演出の状態を設定
	/// </summary>
	/// <param name="effectState">演出の状態</param>
	void SetEffectState(int effectState) { effectState_ = effectState; }

private:
	/// <summary>
	/// シーン切り替えの条件
	/// </summary>
	/// <param name="sceneNum">シーン番号</param>
	void SceneChange(int& sceneNum);

	// 画面演出やライトの演出
	void EffectUpdate();

	// モデル追加
	void AddModel(Model* model) { models_.push_back(model); }

public:// 定数
	// 暗転時のポイントライトの減衰率
	float kMaxPointLightDecay = 16.0f;

	// ラジアルブラーの強さ
	float kBlurStrength = 0.01f;
	// ヴィネットの大きさ
	float kMaxVignetteScale = 100.0f;
	// ヴィネットの初期の大きさ
	float kDefaultVignetteScale = 1000.0f;

private:// エンジン機能
	TextureManager* textureManager_;
	ModelManager* modelManager_;
	Audio* audio_;
	LevelManager* levelManager_;

private:// プライベートなメンバ変数
	// 登録用のモデル
	std::vector<Model*> models_;
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

	// スタート演出
	std::unique_ptr<StartEvent> startEvent_;
	// クリア演出
	std::unique_ptr<ClearEvent> clearEvent_;
	// 死亡演出
	std::unique_ptr<DeadEvent> deadEvent_;

	// UI
	std::array<Sprite, 6> guideUI_;

	// BGM
	uint32_t BGM_;

	// ポイントライトの減衰率
	float lightDecay_;
	// ブラーの強さ
	float blurStrength_;
	// ヴィネットの大きさ
	float vignetteScale_;
	// 時間の速さ
	float timeScale_;

	// 演出の状態
	int effectState_ = Normal;
};