#pragma once
#include "IScene.h"
#include "Input.h"
#include "Sprite.h"

class GameManager;

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

private:
	Input* input_;

	// UI
	std::array<std::unique_ptr<Sprite>, 3> guideUI_;
};