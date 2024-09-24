#pragma once
// Manager
#include "ModelManager.h"
#include "TextureManager.h"
// Parts
#include "LeftHand.h"
#include "RightHand.h"

class EnemyManager;
class Player;
/// <summary>
/// タイタンのパーツを管理するクラス
/// </summary>
class PartsManager {
public:
	PartsManager() = default;
	~PartsManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WorldTransform* object3d,Camera* camera, EnemyManager* enemyManager, int id);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// 
	/// User Method
	/// 

private:
	// 右手の生成
	void CreateRightHand(int id);
	// 左手の生成
	void CreateLeftHand(int id);
	// 弱点の生成
	void CreateWeakness(int id);

public:
#pragma region Getter

#pragma endregion

#pragma region Setter
	// 自機のアドレスを設定
	void SetPlayer(Player* player) { player_ = player; }
	// 敵管理クラスのアドレスを設定
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }
#pragma endregion

private:// エンジン機能
	ModelManager* modelManager_;
	TextureManager* textureManager_;

private:
	// 使用するモデル
	std::vector<Model*> models_;

	// ボスのパーツ
	std::vector<IPart*> parts_;

	// ボスの体(親子関係の親に当たるオブジェクト)
	WorldTransform* rootObject_;

	// 敵管理クラスのアドレス
	EnemyManager* enemyManager_;
	// カメラのアドレス
	Camera* camera_;
	// 自機のアドレス
	Player* player_;
};