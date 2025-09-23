#pragma once
#include "IScene.h"
#include "Input.h"
#include "Sprite.h"
#include "Camera.h"
#include "World/World.h"

class GameManager;
/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public IScene
{
public:
	~TitleScene()override = default;

	// 初期化
	void Initialize() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
	// 解放処理
	void Finalize()override;

private:// エンジン機能
	Input* input_;
	std::unique_ptr<Camera> camera_;
private:
	World mWorld;

	bool mIsEnd = false;

	int mRunnningNodeId = 0;
};