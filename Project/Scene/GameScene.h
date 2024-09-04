#pragma once
#include "IScene.h"
#include "Audio.h"
#include "TextureManager.h"
#include "ModelManager.h"

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

private:// プライベートなメンバ変数
	// 基本機能
	TextureManager* textureManager_;
	ModelManager* modelManager_;
	Audio* audio_;
};

