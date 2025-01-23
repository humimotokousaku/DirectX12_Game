#pragma once
#include "CollisionManager.h"
#include "LevelManager.h"
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
#include "GameObjectManager.h"

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

	/// <summary>
	/// 常に処理する
	/// </summary>
	void ConstantUpdate(int& sceneNum);

	/// <summary>
	/// 特定条件でのみ処理する
	/// </summary>
	void ConditionallyUpdate();

	/// <summary>
	/// ジャスト回避のチュートリアルUI描画
	/// </summary>
	void JustEvasionDrawUI();

	/// <summary>
	/// 攻撃のUI描画
	/// </summary>
	void AttackDrawUI();

	/// <summary>
	/// 演出の状態を設定
	/// </summary>
	/// <param name="effectState">演出の状態</param>
	void SetEffectState(const int& effectState) { effectState_ = effectState; }

	// 自機の弾を追加
	void AddPlayerBullet(PlayerBullet* playerBullet) { playerBullets_.push_back(playerBullet); }
	// 自機の弾リストを取得
	std::list<PlayerBullet*> GetPlayerBulletList() { return playerBullets_; }

private:
	/// <summary>
	/// シーン切り替えの条件
	/// </summary>
	/// <param name="sceneNum">シーン番号</param>
	void SceneChange(int& sceneNum);

	// 画面演出やライトの演出
	void EffectUpdate();

	// モデル追加
	void AddModel(std::string modelName,Model* model) { models_.insert(std::make_pair(modelName, std::move(model))); }

public:// 定数
	// 暗転時のポイントライトの減衰率
	const float kMaxPointLightDecay = 16.0f;

	// ラジアルブラーの強さ
	const float kBlurStrength = 0.01f;
	// ヴィネットの大きさ
	const float kMaxVignetteScale = 100.0f;
	// ヴィネットの初期の大きさ
	const float kDefaultVignetteScale = 1000.0f;
	// ジャスト回避時の振動の大きさ
	const float kMaxVibrationStrength = 65535.0f;

	// 操作UIの数
	const static uint32_t kMaxGuideUINum = 6;

private:// エンジン機能
	TextureManager* textureManager_;
	ModelManager* modelManager_;
	Audio* audio_;
	LevelManager* levelManager_;

private:// プライベートなメンバ変数
	// 登録用のモデル
	std::map<std::string, Model*> models_;

	// スコア
	Score* score_;

	// ゲームオブジェクトマネージャー
	GameObjectManager* gameObjectManager_;

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
	std::array<Sprite, kMaxGuideUINum> guideUI_;

	// ボタンの残像アニメーション
	std::array<Animation, 3> buttonAfterImageAnim_;

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
	// コントローラーの振動の強さ
	float vibrationStrength_;

	// 演出の状態
	int effectState_ = Normal;
};