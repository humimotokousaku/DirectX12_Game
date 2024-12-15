#pragma once
#include "IScene.h"
#include "Input.h"
#include "Sprite.h"
#include "TitleEvent.h"

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

private:
	// タイトル演出
	std::unique_ptr<TitleEvent> titleEvent_;

	// UI
	std::array<std::unique_ptr<Sprite>, 2> guideUI_;
};