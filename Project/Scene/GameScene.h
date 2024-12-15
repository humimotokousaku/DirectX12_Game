#pragma once
#include "IScene.h"
#include "GameSystem.h"
#include "GameObjectManager.h"

class GameManager;
/// <summary>
/// ゲームシーン
/// </summary>
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

private:// プライベートなメンバ変数
	// ゲームシステム
	std::unique_ptr<GameSystem> gameSystem_;

	// ゲームオブジェクトマネージャー
	GameObjectManager* gameObjectManager_;
};

