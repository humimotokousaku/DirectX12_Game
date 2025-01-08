#pragma once
#include "Sprite.h"
#include "PostEffectManager.h"
#include "BaseEnemy.h"
#include "PlayerConfig.h"
#include "RailCamera.h"

/// <summary>
/// マルチロックオン機能
/// </summary>
class MultiLockOnSystem {
public:// 構造体
	struct MultiLockOnData {
		std::shared_ptr<Sprite> reticleSprite;  // 2Dレティクル
		std::array<Animation, 2> reticleAnim;	// ロックオン時のアニメーション
		int enemyId;
		bool isActive;
	};
public:
	MultiLockOnSystem() = default;
	~MultiLockOnSystem() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Player* player, Camera* camera, std::list<BaseEnemy*>* enemys, GameSystem* gameSystem, Model* model);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:// プライベートなメンバ関数
	/// <summary>
	/// 弾の発射処理
	/// </summary>
	void Shot();

	/// <summary>
	/// ロックオンの更新処理
	/// </summary>
	void LockOnUpdate();

	/// <summary>
	/// ロックオンされた敵を削除する処理
	/// </summary>
	void EraseLockedList();

	/// </summary>
	/// ワールド座標からスクリーン座標に変換
	/// </summary>
	/// <param name="worldPos"></param>
	/// <returns></returns>
	Vector2 ConvertWorld2Screen(Vector3 worldPos);

	/// </summary>
	/// 画面内にオブジェクトがいるかを検出
	/// </summary>
	/// <param name="worldPos"></param>
	/// <returns></returns>
	bool IsObjectInScreen(Vector3 worldPos);

public:// GetterとSetter
#pragma region Setter
	// レールカメラのアドレスを設定
	void SetRailCamera(RailCamera* railCamera) { railCamera_ = railCamera; }
	// 全敵のアドレスを設定
	void SetEnemyList(std::list<BaseEnemy*>* enemys) { enemys_ = enemys; }
	// 全敵のIDを設定
	void SetEnemyIdList(std::vector<int>* enemyIdList) { enemyIdList_ = enemyIdList; }
#pragma endregion

#pragma region Getter
	/// <summary>
	/// ロックオン可能状態かを取得する
	/// </summary>
	/// <returns></returns>
	const bool& GetIsActive() { return player_->GetBulletGauge().isMax; }
#pragma endregion

private:// 定数
	// 同時ロックオン可能数
	const int kMaxLockOnNum = 20;

	// 弾の発射間隔
	const int kShotInterval = 3;

private:// エンジン機能
	// ポストエフェクト
	PostEffectManager* postEffectManager_;
	// オーディオ
	Audio* audio_;
	// 入力
	Input* input_;

private:
	// カメラのアドレス
	Camera* camera_;
	// 追従カメラのアドレス
	RailCamera* railCamera_;
	// 全ての敵
	std::list<BaseEnemy*>* enemys_;
	// ゲームシステム
	GameSystem* gameSystem_;
	// 自機
	Player* player_;

	// 弾の見た目
	Model* model_;
	// 射撃SE
	uint32_t shotSE_;

	// マルチロックオンの情報
	std::vector<MultiLockOnData> multiLockOnDatas_;
	// ロックオンされている敵のID
	std::vector<int> lockedEnemyIdList_;
	// 全敵のIDリスト
	std::vector<int>* enemyIdList_;

	// 経過時間
	float currentFrame_;

	// 一度に発射した回数
	int shotNum_;
	// 一度に発射する予定の弾数
	int shotGoal_;

	// 弾を発射しているか
	bool isShot_;
};