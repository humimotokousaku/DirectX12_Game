#pragma once
#include "IScene.h"
#include "EnemyBullet.h"
#include "Enemy.h"
#include "PlayerBullet.h"
#include "Player.h"
#include "RailCamera.h"
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

	// レールカメラに沿って自機を動かす
	void UpdatePlayerPosition(float t);

	// 敵の生成
	void SpawnEnemy(Vector3 pos);

	// 敵の出現情報の読み込み
	void LoadEnemyPopData();
	// 敵の出現情報をもとに更新
	void UpdateEnemyPopCommands();

	// 自機の弾を追加
	void AddPlayerBullet(PlayerBullet* playerBullet);
	// 敵の弾を追加
	void AddEnemyBullet(EnemyBullet* enemyBullet);

public:// パブリックなメンバ変数
	// 線分の数
	static const size_t segmentCount = 100;

private:// プライベートなメンバ変数
	// カメラ
	std::unique_ptr<Camera> camera_;

	// 自機
	std::unique_ptr<Player> player_;
	// 自弾
	std::list<PlayerBullet*> playerBullets_;
	// 敵
	std::list<Enemy*> enemy_;
	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;
	// 衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;
	// カメラレール
	std::unique_ptr<RailCamera> railCamera_ = nullptr;
	// skybox
	std::unique_ptr<Cube> cube_;

	// 敵の発生時間や座標などのCSVファイル
	std::stringstream enemyPopCommands_;
	// 敵が発生待機中か
	bool isWait_ = false;
	// 敵が発生するまでの時間
	int32_t waitTime_ = 0;

	// スプライン曲線制御点（通過点）
	//std::vector<Vector3> controlPoints_;
	// 線分で描画する用の頂点リスト
	std::vector<Vector3> pointsDrawing_;
	// 移動ルートの線(デバッグ用)
	std::array<std::unique_ptr<Line>, segmentCount> line_;
	Vector3 target_;
	float t_;
	float targetT_;
	bool isMoveCamera_;
};

